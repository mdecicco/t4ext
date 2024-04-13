#include <script/TypeScriptAPI.h>
#include <script/Calling.h>
#include <Client.h>

#include <events/Timeout.h>
#include <events/TypeScriptTimeout.h>

#include <utils/Buffer.hpp>
#include <utils/Arguments.h>


#include <libplatform/libplatform.h>

#include <direct.h>
#include <filesystem>
#include <sys/stat.h>
#include <windows.h>

namespace tsc {
    bool compileEntryScript(t4ext::TypeScriptAPI* api) {
        utils::String compilerPath = api->getGameBasePath() + "/scripts/compiler/tsc.exe";
        utils::String outputPath = api->getGameBasePath() + "/scripts/output/package.js";

        // check if input file actually exists
        struct stat inputStat;
        if (stat(api->getScriptEntrypointPath().c_str(), &inputStat) != 0) {
            api->error("Script entrypoint '%s' does not exist", api->getScriptEntrypointPath().c_str());
            return false;
        }

        // todo: check for file modifications to see if we can exit early

        struct stat outputStat;
        if (stat(outputPath.c_str(), &outputStat) == 0) {
            // delete the previous script
            if (std::remove(outputPath.c_str()) != 0) {
                api->error("Failed to delete previously compiled script");
                return false;
            }
        }

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        SECURITY_ATTRIBUTES saAttr;

        ZeroMemory(&saAttr, sizeof(saAttr));
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;
        
        HANDLE readPipe;
        HANDLE writePipe;
        utils::Array<utils::String> programOutput;

        // Create a pipe for the child process's STDOUT. 
        if (CreatePipe(&readPipe, &writePipe, &saAttr, 0) == 0) {
            api->error("Failed to create output pipe for compiler process");
            return false;
        }

        // Ensure the read handle to the pipe for STDOUT is not inherited.
        if (SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0) == 0) {
            api->error("Call to SetHandleInformation failed");
            CloseHandle(readPipe);
            CloseHandle(writePipe);
            return false;
        }

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.hStdError = writePipe;
        si.hStdOutput = writePipe;
        si.dwFlags |= STARTF_USESTDHANDLES;

        ZeroMemory(&pi, sizeof(pi));

        utils::String args = "\"" + compilerPath + "\" -p ./tsconfig.json -outFile ./output/package.js";
        utils::String cwd = api->getGameBasePath() + "/scripts";
        if (CreateProcessA(compilerPath.c_str(), (LPSTR)args.c_str(), NULL, NULL, TRUE, 0, NULL, cwd.c_str(), &si, &pi) == 0) {
            api->error("Failed to spawn compiler process");
            CloseHandle(readPipe);
            CloseHandle(writePipe);
            return false;
        }

        api->log("Waiting for compiler...");
        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(writePipe);
        
        char chBuf[4096] = { 0 };
        utils::String lineBuf;

        while (true) {
            DWORD dwRead;
            if (ReadFile(readPipe, chBuf, 4096, &dwRead, NULL) == 0 || dwRead == 0) break;

            for (utils::u32 i = 0;i < 4096 && chBuf[i] != 0;i++) {
                if (chBuf[i] == '\n' || chBuf[i] == '\r') {
                    if (lineBuf.size() > 0) programOutput.push(lineBuf);
                    lineBuf = "";
                    continue;
                }

                lineBuf += chBuf[i];
            }

            memset(chBuf, 0, dwRead);
        }

        CloseHandle(readPipe);

        if (lineBuf.size() > 0) {
            programOutput.push(lineBuf);
        }

        utils::IWithLogging tscLog("tsc");
        tscLog.subscribeLogger(api);
        if (programOutput.size() > 0) {
            for (utils::String& log : programOutput) {
                tscLog.log(log);
            }
        }

        DWORD exitCode = 0;
        if (GetExitCodeProcess(pi.hProcess, &exitCode) == 0) {
            api->error("Compilation may have succeeded, but we were unable to get the exit code of the compiler process");
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return false;
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if (exitCode != 0) {
            api->error("Compiler failed with exit code %d", exitCode);
            return false;
        }

        // Make sure it actually generated the file
        if (stat(outputPath.c_str(), &outputStat) != 0) {
            api->error("Compilation apparently succeeded, but the output file does not exist");
            return false;
        }

        api->log("Compilation succeeded");

        return true;
    }
};

namespace t4ext {
    void consoleLog(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Client* c = utils::Singleton<Client>::Get();
        u32 argCount = args.Length();
        for (u32 i = 0;i < argCount;i++) {
            v8::String::Utf8Value str(args.GetIsolate(), args[i]);
            c->log("%s", *str);
        }
    }
    void consoleWarn(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Client* c = utils::Singleton<Client>::Get();
        u32 argCount = args.Length();
        for (u32 i = 0;i < argCount;i++) {
            v8::String::Utf8Value str(args.GetIsolate(), args[i]);
            c->warn("%s", *str);
        }
    }
    void consoleError(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Client* c = utils::Singleton<Client>::Get();
        u32 argCount = args.Length();
        for (u32 i = 0;i < argCount;i++) {
            v8::String::Utf8Value str(args.GetIsolate(), args[i]);
            c->error("%s", *str);
        }
    }
    void defineFunc(const v8::FunctionCallbackInfo<v8::Value>& args) {
        TypeScriptAPI* api = (TypeScriptAPI*)args.Data().As<v8::External>()->Value();
        v8::Isolate* isolate = args.GetIsolate();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Local<v8::Value> moduleId = args[0].As<v8::String>();
        v8::Local<v8::Array> dependencies = args[1].As<v8::Array>();
        v8::Local<v8::Value> initFuncOrExportObj = args[2];

        v8::String::Utf8Value idStr(isolate, moduleId);

        TypeScriptAPI::ModuleInfo* mod = new TypeScriptAPI::ModuleInfo;
        mod->id = *idStr;
        
        for (u32 i = 0;i < dependencies->Length();i++) {
            v8::String::Utf8Value depId(isolate, dependencies->Get(ctx, i).ToLocalChecked());
            mod->dependencies.push(*depId);
        }

        if (initFuncOrExportObj->IsFunction()) mod->factory.Reset(isolate, initFuncOrExportObj.As<v8::Function>());
        else if (!initFuncOrExportObj.IsEmpty()) mod->exports.Reset(isolate, initFuncOrExportObj.As<v8::Object>());
        else mod->exports.Reset(isolate, v8::Object::New(isolate));

        api->defineModule(mod);
    }
    void requireFunc(const v8::FunctionCallbackInfo<v8::Value>& args) {
        args.GetIsolate()->ThrowError(v8::String::NewFromUtf8Literal(args.GetIsolate(), "The 'require' function is not implemented. Bug stinkee2 about it"));
    }
    void eventPoll(const v8::FunctionCallbackInfo<v8::Value>& args) {
        bool doContinue = true;

        TypeScriptAPI* api = (TypeScriptAPI*)args.Data().As<v8::External>()->Value();
        api->handleEvents();
        
        utils::Singleton<TimeoutEventType>::Get()->processTimeouts();
        
        args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), doContinue));
    }
    void setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if (args.Length() == 0) return;
        TypeScriptAPI* api = (TypeScriptAPI*)args.Data().As<v8::External>()->Value();
        v8::Isolate* isolate = args.GetIsolate();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        if (!args[0]->IsFunction()) {
            args.GetIsolate()->ThrowError(v8::String::NewFromUtf8Literal(args.GetIsolate(), "setTimeout expects the first parameter to be a callback function"));
            return;
        }

        if (args.Length() >= 2 && !args[1]->IsNumber()) {
            args.GetIsolate()->ThrowError(v8::String::NewFromUtf8Literal(args.GetIsolate(), "setTimeout expects the second parameter to be a duration in milliseconds"));
            return;
        }

        v8::Local<v8::Function> callback = args[0].As<v8::Function>();
        v8::Local<v8::Number> duration = args[1].As<v8::Number>();


        u32 dur = duration.IsEmpty() ? 0 : u32(duration->Value());

        TypeScriptTimeoutData* data = new TypeScriptTimeoutData(api, isolate, callback, dur, false);
        u32 id = utils::Singleton<TimeoutEventType>::Get()->createTimeout(data);
        args.GetReturnValue().Set(v8::Number::New(isolate, id));
    }
    void setInterval(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if (args.Length() == 0) return;
        TypeScriptAPI* api = (TypeScriptAPI*)args.Data().As<v8::External>()->Value();
        v8::Isolate* isolate = args.GetIsolate();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        if (!args[0]->IsFunction()) {
            args.GetIsolate()->ThrowError(v8::String::NewFromUtf8Literal(args.GetIsolate(), "setInterval expects the first parameter to be a callback function"));
            return;
        }

        if (args.Length() >= 2 && !args[1]->IsNumber()) {
            args.GetIsolate()->ThrowError(v8::String::NewFromUtf8Literal(args.GetIsolate(), "setInterval expects the second parameter to be a duration in milliseconds"));
            return;
        }

        v8::Local<v8::Function> callback = args[0].As<v8::Function>();
        v8::Local<v8::Number> duration = args[1].As<v8::Number>();

        u32 dur = duration.IsEmpty() ? 0 : u32(duration->Value());
        TypeScriptTimeoutData* data = new TypeScriptTimeoutData(api, isolate, callback, dur, true);
        u32 id = utils::Singleton<TimeoutEventType>::Get()->createTimeout(data);

        args.GetReturnValue().Set(v8::Number::New(isolate, id));
    }


    TypeScriptAPI::TypeScriptAPI() {
        m_platform = nullptr;
        m_isolate = nullptr;
        m_arrayBufferAllocator = nullptr;
        m_didInitialize = false;
    }

    TypeScriptAPI::~TypeScriptAPI() {
    }

    TypeScriptAPI::ModuleInfo* TypeScriptAPI::getModule(const utils::String& moduleId) {
        auto it = m_modules.find(moduleId);
        if (it == m_modules.end()) return nullptr;
        return it->second;
    }

    void TypeScriptAPI::defineModule(ModuleInfo* module) {
        m_modules[module->id] = module;
    }

    bool TypeScriptAPI::initialize() {
        if (!v8::V8::InitializeICUDefaultLocation(m_exePath.c_str())) {
            error("Call to V8::InitializeICUDefaultLocation failed");
            return false;
        }
        v8::V8::InitializeExternalStartupData(m_pathBase.c_str());

        m_platform = v8::platform::NewDefaultPlatform();
        v8::V8::InitializePlatform(m_platform.get());

        if (!v8::V8::Initialize()) {
            error("Failed to initialize v8");
            v8::V8::DisposePlatform();
            return false;
        }

        m_arrayBufferAllocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

        v8::Isolate::CreateParams cp;
        cp.array_buffer_allocator = m_arrayBufferAllocator;

        m_isolate = v8::Isolate::New(cp);
        if (!m_isolate) {
            error("Failed to create v8 isolate");
            v8::V8::Dispose();
            v8::V8::DisposePlatform();
            delete m_arrayBufferAllocator;
            return false;
        }

        m_didInitialize = true;
        return true;
    }
    
    bool TypeScriptAPI::commitBindings() {
        log("Generating globals.d.ts...");
        if (!generateDefs()) return false;

        log("Compiling script entrypoint...");
        if (!tsc::compileEntryScript(this)) {
            // errors should've already been emitted
            return false;
        }

        return true;
    }
    
    bool TypeScriptAPI::shutdown() {
        if (!m_didInitialize) return true;

        for (auto it = m_modules.begin();it != m_modules.end();++it) {
            it->second->exports.Reset();
            it->second->factory.Reset();
            delete it->second;
        }

        m_eventMutex.lock();
        for (IEventType* tp : m_eventTypes) delete tp;
        for (IEvent* e : m_events) delete e;
        m_eventTypes.clear();
        m_events.clear();
        m_eventMutex.unlock();
        
        v8::V8::Dispose();
        v8::V8::DisposePlatform();
        delete m_arrayBufferAllocator;

        m_arrayBufferAllocator = nullptr;
        m_platform = nullptr;
        m_isolate = nullptr;
        m_didInitialize = false;

        return true;
    }

    bool TypeScriptAPI::executeEntry() {
        if (!m_didInitialize) return false;

        log("Executing the entrypoint...");

        utils::Buffer* buf = utils::Buffer::FromFile(m_pathBase + "/scripts/output/package.js");
        if (!buf) {
            error("Failed to open script '%s'");
            return false;
        }

        bool didSucceed = false;

        {
            v8::Locker locker(m_isolate);
            v8::Isolate::Scope isolate_scope(m_isolate);
            v8::HandleScope handle_scope(m_isolate);
            v8::Local<v8::Context> context = v8::Context::New(m_isolate);
            v8::Context::Scope context_scope(context);

            setupContext(context);

            v8::Local<v8::String> source;
            if (v8::String::NewFromUtf8(m_isolate, (const char*)buf->data()).ToLocal(&source)) {
                didSucceed = execute(context, source);
            }

            if (didSucceed) {
                ModuleInfo* mainModule = getModule("main");
                if (!mainModule) {
                    error("Successfully compiled and initialized the entrypoint, but failed to obtain the main module");
                    didSucceed = false;
                } else {
                    loadModule(context, mainModule);
                }
            }
        }

        delete buf;
        return didSucceed;
    }

    bool TypeScriptAPI::executeScriptFile(const char* path) {
        utils::Buffer* buf = utils::Buffer::FromFile(path);
        if (!buf) {
            error("Failed to open script '%s'");
            return false;
        }

        bool didSucceed = executeScriptMemory((const char*)buf->data());

        delete buf;
        return didSucceed;
    }

    bool TypeScriptAPI::executeScriptMemory(const char* buf) {
        bool didSucceed = false;

        {
            v8::Locker locker(m_isolate);
            v8::Isolate::Scope isolate_scope(m_isolate);
            v8::HandleScope handle_scope(m_isolate);
            v8::Local<v8::Context> context = v8::Context::New(m_isolate);
            v8::Context::Scope context_scope(context);

            setupContext(context);

            v8::Local<v8::String> source;
            if (v8::String::NewFromUtf8(m_isolate, buf).ToLocal(&source)) {
                didSucceed = execute(context, source);
            }
        }

        return didSucceed;
    }

    void TypeScriptAPI::logException(const v8::Local<v8::Context>& context, const v8::TryCatch& tc) {
        v8::String::Utf8Value exc_str(m_isolate, tc.Exception());
        error(*exc_str);

        v8::Local<v8::Value> trace;
        if (tc.StackTrace(context).ToLocal(&trace)) {
            v8::String::Utf8Value trace_str(m_isolate, trace);
            error(*trace_str);
        }
    }

    bool TypeScriptAPI::execute(const v8::Local<v8::Context>& context, const v8::Local<v8::String>& source) {
        bool didSucceed = true;

        v8::TryCatch tc(m_isolate);
        v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
        if (!tc.HasCaught()) {
            script->Run(context);
            if (tc.HasCaught()) {
                error("Caught exception while running script");
                logException(context, tc);
                didSucceed = false;
            }
        } else {
            error("Caught exception in script");
            logException(context, tc);
            didSucceed = false;
        }

        return didSucceed;
    }

    v8::Local<v8::Object> TypeScriptAPI::loadModule(const v8::Local<v8::Context>& context, ModuleInfo* module) {
        if (module->exports.IsEmpty()) {
            v8::Local<v8::Object> exports = v8::Object::New(m_isolate);

            if (module->factory.IsEmpty()) {
                warn("Nothing to do for module '%s'", module->id.c_str());
                module->exports.Reset(m_isolate, exports);
                return exports;
            }

            utils::Array<v8::Local<v8::Value>> deps;
            for (u32 i = 0;i < module->dependencies.size();i++) {
                utils::String& depId = module->dependencies[i];
                if (depId == "require") {
                    deps.push(v8::FunctionTemplate::New(m_isolate, requireFunc)->GetFunction(context).ToLocalChecked());
                } else if (depId == "exports") {
                    deps.push(exports);
                } else {
                    ModuleInfo* depMod = getModule(depId);
                    if (!depMod) {
                        // In some cases, the compiled code of a module can may be placed "inline" at the locations
                        // where they are imported, rather than going through the 'define' function. In these cases,
                        // the dependency won't have any exports
                        //
                        // Keep an eye on this, even it's possible the behavior required in that scenario is more
                        // nuanced than just "don't pass the corresponding argument to the factory functions"
                        
                        continue;
                    }

                    deps.push(loadModule(context, depMod));
                }
            }

            v8::Local<v8::Function> factory = module->factory.Get(m_isolate);
            factory->Call(context, context->Global(), deps.size(), deps.data());

            module->exports.Reset(m_isolate, exports);
            return exports;
        }

        return module->exports.Get(m_isolate);
    }

    void TypeScriptAPI::setupContext(const v8::Local<v8::Context>& context) {
        v8::Local<v8::Object> global = context->Global();
        v8::Local<v8::String> key;
        
        key = v8::String::NewFromUtf8Literal(m_isolate, "console");
        v8::Local<v8::Object> console = global->Get(context, key).ToLocalChecked().As<v8::Object>();
        
        key = v8::String::NewFromUtf8Literal(m_isolate, "log");
        console->Set(context, key, v8::FunctionTemplate::New(m_isolate, consoleLog)->GetFunction(context).ToLocalChecked());
        key = v8::String::NewFromUtf8Literal(m_isolate, "warn");
        console->Set(context, key, v8::FunctionTemplate::New(m_isolate, consoleWarn)->GetFunction(context).ToLocalChecked());
        key = v8::String::NewFromUtf8Literal(m_isolate, "error");
        console->Set(context, key, v8::FunctionTemplate::New(m_isolate, consoleError)->GetFunction(context).ToLocalChecked());

        v8::Local<v8::External> self = v8::External::New(m_isolate, this);
        global->Set(
            context,
            v8::String::NewFromUtf8Literal(m_isolate, "define"),
            v8::FunctionTemplate::New(m_isolate, defineFunc, self)->GetFunction(context).ToLocalChecked()
        );

        global->Set(
            context,
            v8::String::NewFromUtf8Literal(m_isolate, "event_poll"),
            v8::FunctionTemplate::New(m_isolate, eventPoll, self)->GetFunction(context).ToLocalChecked()
        );

        global->Set(
            context,
            v8::String::NewFromUtf8Literal(m_isolate, "setTimeout"),
            v8::FunctionTemplate::New(m_isolate, setTimeout, self)->GetFunction(context).ToLocalChecked()
        );

        global->Set(
            context,
            v8::String::NewFromUtf8Literal(m_isolate, "setInterval"),
            v8::FunctionTemplate::New(m_isolate, setInterval, self)->GetFunction(context).ToLocalChecked()
        );

        v8::Local<v8::Object> t4 = v8::Object::New(m_isolate);

        t4->SetAccessorProperty(
            v8::String::NewFromUtf8Literal(m_isolate, "elapsedTime"),
            v8::FunctionTemplate::New(m_isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args){
                args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), gClient::Get()->elapsedTime()));
            }, self)->GetFunction(context).ToLocalChecked(),
            v8::Local<v8::Function>(),
            v8::PropertyAttribute::ReadOnly
        );

        for (Function* f : m_globalFunctions) {
            v8::Local<v8::External> data = v8::External::New(m_isolate, f);
            t4->Set(
                context,
                v8::String::NewFromUtf8(m_isolate, f->getName().c_str()).ToLocalChecked(),
                v8::FunctionTemplate::New(m_isolate, HostCallHandler, data)->GetFunction(context).ToLocalChecked()
            );
        }

        global->Set(context, v8::String::NewFromUtf8Literal(m_isolate, "t4"), t4);
    }

    bool TypeScriptAPI::generateDefs() {
        // generate globals.d.ts
        char buf[256] = { 0 };
        utils::Buffer out;
        u32 indent = 0;
        auto l = [this, &out, &indent, &buf](const char* fmt, ...) {
            utils::String ln;
            for (u32 i = 0;i < indent;i++) ln += "    ";

            va_list ap;
            va_start(ap, fmt);
            vsnprintf(buf, 256, fmt, ap);
            va_end(ap);
            
            ln += buf;
            ln += "\n";

            out.write(ln.c_str(), ln.size());
        };

        auto tpName = [](DataType* tp, bool isPtr = false, bool isNullable = false) -> utils::String {
            if (tp->getPrimitiveType() == Primitive::pt_char && isPtr) return isNullable ? "string | null" : "string";

            if (isPtr && isNullable) return tp->getName() + " | null";
            return tp->getName();
        };

        l("declare namespace t4 {");
        indent++;
        
        bool isFirstType = true;
        bool lastWasPrim = false;
        for (DataType* tp : m_types) {
            if (!isFirstType && !(lastWasPrim && tp->isPrimitive() && tp->getPrimitiveType() != Primitive::pt_enum)) l("");
            isFirstType = false;

            if (tp->isPrimitive()) {
                // this may not always be applicable in the future, but it is now
                if (tp->getPrimitiveType() == Primitive::pt_char) l("type char = number | string;");
                else if (tp->getPrimitiveType() == Primitive::pt_enum) {
                    l("enum %s {", tp->getName().c_str());
                    indent++;
                    
                    utils::Array<DataTypeField>& fields = tp->getFields();
                    for (u32 i = 0;i < fields.size();i++) {
                        DataTypeField& f = fields[i];
                        l("%s = %lu%s", f.name.c_str(), f.offset, i == fields.size() - 1 ? "" : ",");
                    }

                    indent--;
                    l("}");
                    continue;
                } else l("type %s = number;", tp->getName().c_str());
                lastWasPrim = true;
                continue;
            }

            lastWasPrim = false;
            
            utils::String extends = "";
            utils::Array<DataType*>& bases = tp->getBases();
            if (bases.size() > 1) {
                MessageBoxA(
                    nullptr,
                    utils::String::Format("DataType '%s' inherits multiple other types, which is currently unsupported", tp->getName().c_str()).c_str(),
                    "Unsupported Behavior",
                    MB_OK
                );
                exit(1);
            } else if (bases.size() == 1) {
                extends = utils::String::Format("extends %s ", bases[0]->getName().c_str());
            }

            l("/**");
            l(" * @size 0x%X (%d bytes)", tp->getSize(), tp->getSize());
            l(" */");
            l("class %s %s{", tp->getName().c_str(), extends.c_str());
            indent++;
            
            utils::Array<DataTypeField>& fields = tp->getFields();
            bool addSpace = false;
            for (DataTypeField& f : fields) {
                if (addSpace) l("");
                addSpace = true;

                u32 sz = f.flags.is_pointer ? 4 : f.type->getSize();

                l("/**");
                l(" * @offset 0x%X (%d bytes)", f.offset, f.offset);
                l(" * @size 0x%X (%d bytes)", sz, sz);
                l(" */");
                l("%s%s: %s;", f.flags.is_readonly ? "readonly " : "", f.name.c_str(), tpName(f.type, f.flags.is_pointer, f.flags.is_nullable).c_str());
            }

            utils::Array<Function*>& methods = tp->getMethods();
            for (Function* m : methods) {
                FunctionSignature& sig = m->getSignature();

                utils::String str = m->getName();
                str += "(";

                utils::Array<FunctionArgument>& args = sig.getArgs();
                bool isFirst = true;
                for (FunctionArgument& a : args) {
                    if (!isFirst) str += ", ";
                    isFirst = false;

                    str += a.name + ": " + tpName(a.type, a.flags.is_pointer, a.flags.is_nullable);
                }

                str += utils::String("): ") + (sig.getRetTp() ? tpName(sig.getRetTp(), sig.returnsPointer(), sig.returnsPointer()) : "void").c_str() + ";";
                
                l("/**");
                l(" * @address 0x%X", m->getAddress());
                l(" */");
                l(str.c_str());
            }

            indent--;
            l("}");
        }

        if (m_types.size() > 0 && m_globalFunctions.size() > 0) l("");

        for (Function* f : m_globalFunctions) {
            FunctionSignature& sig = f->getSignature();
            utils::String str = utils::String("function ") + f->getName();
            str += "(";

            utils::Array<FunctionArgument>& args = sig.getArgs();
            bool isFirst = true;
            for (FunctionArgument& a : args) {
                if (!isFirst) str += ", ";
                isFirst = false;

                str += a.name + ": " + tpName(a.type, a.flags.is_pointer, a.flags.is_nullable);
            }

            str += utils::String("): ") + (sig.getRetTp() ? tpName(sig.getRetTp(), sig.returnsPointer(), sig.returnsPointer()) : "void") + ";";
            
            l("/**");
            l(" * @address 0x%X", f->getAddress());
            l(" */");
            l(str.c_str());
        }

        if (m_globalFunctions.size() > 0 && m_globalVars.size() > 0) l("");

        bool isFirst = true;
        for (GlobalVariable& g : m_globalVars) {
            if (!isFirst) l("");
            isFirst = false;

            u32 sz = g.flags.is_pointer ? 4 : g.type->getSize();
            
            l("/**");
            l(" * @address 0x%X", g.address);
            l(" * @size 0x%X (%d bytes)", sz, sz);
            l(" */");
            l("const %s: %s;", g.name.c_str(), tpName(g.type, g.flags.is_pointer, g.flags.is_nullable).c_str());
        }

        indent--;
        l("}");

        if (!out.save(m_pathBase + "/scripts/globals.d.ts")) {
            error("Failed to write globals.d.ts file");
            return false;
        }

        return true;
    }
};