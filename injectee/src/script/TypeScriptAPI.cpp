#include <script/TypeScriptAPI.h>
#include <script/IScriptAPI.hpp>
#include <script/Calling.h>
#include <script/TSHelpers.h>
#include <script/TSInternal.h>

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
#include <stdarg.h>

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
                    if (lineBuf.size() == 0) continue;
                    programOutput.push(lineBuf);
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
    TypeScriptCallback::TypeScriptCallback(const v8::Local<v8::Function>& func) {
        m_callback.Reset(func->GetIsolate(), func);
    }

    TypeScriptCallback::~TypeScriptCallback() {
        m_callback.Reset();
    }
    
    v8::Local<v8::Function> TypeScriptCallback::get(v8::Isolate* isolate) {
        return m_callback.Get(isolate);
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
    
    v8::Isolate* TypeScriptAPI::getIsolate() {
        return m_isolate;
    }

    v8::Local<v8::Context>& TypeScriptAPI::getContext() {
        return m_context;
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

        utils::Buffer* buf = utils::Buffer::FromFile(m_pathBase + "/scripts/output/package.js", true);
        if (!buf) {
            error("Failed to open script '%s'");
            return false;
        }

        bool didSucceed = false;

        {
            v8::Locker locker(m_isolate);
            v8::Isolate::Scope isolate_scope(m_isolate);
            v8::HandleScope handle_scope(m_isolate);
            m_context = v8::Context::New(m_isolate);
            v8::Context::Scope context_scope(m_context);

            setupContext();

            v8::Local<v8::String> source;
            if (v8::String::NewFromUtf8(m_isolate, (const char*)buf->data()).ToLocal(&source)) {
                didSucceed = execute(source);
            }
            
            delete buf;

            if (didSucceed) {
                ModuleInfo* mainModule = getModule("main");
                if (!mainModule) {
                    error("Successfully compiled and initialized the entrypoint, but failed to obtain the main module");
                    didSucceed = false;
                } else {
                    // all aboard
                    m_isReady = true;
                    loadModule(mainModule);
                }
            }
        }

        return didSucceed;
    }

    void TypeScriptAPI::logException(const v8::TryCatch& tc) {
        v8::String::Utf8Value exc_str(m_isolate, tc.Exception());
        error(*exc_str);

        v8::Local<v8::Value> trace;
        if (tc.StackTrace(m_context).ToLocal(&trace)) {
            v8::String::Utf8Value trace_str(m_isolate, trace);
            error(*trace_str);
        }
    }

    bool TypeScriptAPI::isReady() {
        return m_isReady;
    }

    bool TypeScriptAPI::execute(const v8::Local<v8::String>& source) {
        bool didSucceed = true;

        v8::TryCatch tc(m_isolate);
        tc.SetVerbose(true);
        v8::Local<v8::Script> script;
        if (!v8::Script::Compile(m_context, source).ToLocal(&script)) {
            if (tc.HasCaught()) {
                error("Failed to compile script");
                logException(tc);

                log("For debug purposes, the script source was:");
                v8::String::Utf8Value src(m_isolate, source);
                printf("%s", *src);
                didSucceed = false;
            } else {
                error("Failed to compile script, or failed to get compiled script");
            }
            didSucceed = false;
        } else {
            if (!tc.HasCaught()) {
                script->Run(m_context);
                if (tc.HasCaught()) {
                    error("Caught exception while running script");
                    logException(tc);
                    didSucceed = false;
                }
            } else {
                error("Caught exception in script");
                logException(tc);
                didSucceed = false;
            }
        }

        return didSucceed;
    }

    v8::Local<v8::Object> TypeScriptAPI::loadModule(ModuleInfo* module) {
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
                    deps.push(v8::FunctionTemplate::New(m_isolate, requireFunc)->GetFunction(m_context).ToLocalChecked());
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

                    deps.push(loadModule(depMod));
                }
            }

            v8::Local<v8::Function> factory = module->factory.Get(m_isolate);
            factory->Call(m_context, m_context->Global(), deps.size(), deps.data());

            module->exports.Reset(m_isolate, exports);
            return exports;
        }

        return module->exports.Get(m_isolate);
    }

    void TypeScriptAPI::setupContext() {
        v8::Local<v8::Object> global = m_context->Global();
        v8::Local<v8::String> key;
        
        key = v8::String::NewFromUtf8Literal(m_isolate, "console");
        v8::Local<v8::Value> consoleV;
        if (v8GetProp(global, "console", &consoleV)) {
            v8::Local<v8::Object> console = consoleV.As<v8::Object>();
            v8SetProp(console, "log", v8Func(m_isolate, consoleLog));
            v8SetProp(console, "warn", v8Func(m_isolate, consoleWarn));
            v8SetProp(console, "error", v8Func(m_isolate, consoleError));
        }
        
        v8::Local<v8::External> self = v8::External::New(m_isolate, this);
        v8SetProp(global, "define", v8Func(m_isolate, defineFunc, self));
        v8SetProp(global, "event_poll", v8Func(m_isolate, eventPoll, self));
        v8SetProp(global, "setTimeout", v8Func(m_isolate, setTimeout, self));
        v8SetProp(global, "setInterval", v8Func(m_isolate, setInterval, self));
        v8SetProp(global, "clearTimeout", v8Func(m_isolate, clearTimeout, self));
        v8SetProp(global, "clearInterval", v8Func(m_isolate, clearInterval, self));

        v8::Local<v8::Object> t4 = v8::Object::New(m_isolate);

        t4->SetAccessorProperty(
            v8StrLiteral(m_isolate, "elapsedTime"),
            v8Func(m_isolate, +[](const v8::FunctionCallbackInfo<v8::Value>& args){
                args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), gClient::Get()->elapsedTime()));
            }),
            v8::Local<v8::Function>(),
            v8::PropertyAttribute::ReadOnly
        );

        for (ScriptNamespace* ns : m_namespaces) {
            v8::Local<v8::Object> nsObj;
            if (ns->name.size() == 0) nsObj = m_context->Global();
            else if (ns->name == "t4") nsObj = t4;
            else nsObj = v8::Object::New(m_isolate);

            for (Function* f : ns->globalFunctions) {
                v8::Local<v8::External> data = v8::External::New(m_isolate, f);
                v8SetProp(nsObj, f->getName(), v8Func(m_isolate, HostCallHandler, data));
            }

            for (DataType* tp : ns->types) {
                if (tp->getPrimitiveType() != Primitive::pt_enum) continue;

                v8::Local<v8::Object> etp = v8::Object::New(m_isolate);
                utils::Array<DataTypeField>& fields = tp->getFields();
                for (DataTypeField& field : fields) {
                    v8SetProp(etp, field.name, v8::Number::New(m_isolate, field.offset));
                }

                v8SetProp(nsObj, tp->getName(), etp);
            }

            if (ns->name.size() != 0) {
                v8SetProp(global, ns->name, nsObj);
            }
        }
    }

    utils::String tpName(DataType* tp, bool isPtr = false, bool isNullable = false) {
        if (tp->getPrimitiveType() == Primitive::pt_char && isPtr) return isNullable ? "string | null" : "string";

        if (tp->isFunction()) {
            FunctionSignature& sig = tp->getSignature();
            utils::String str;
            str += "(";

            utils::Array<FunctionArgument>& args = sig.getArgs();
            bool isFirst = true;
            for (FunctionArgument& a : args) {
                if (!isFirst) str += ", ";
                isFirst = false;

                str += a.name + ": " + tpName(a.type, a.flags.is_pointer, a.flags.is_nullable);
            }

            str += utils::String(") => ") + (sig.getRetTp() ? tpName(sig.getRetTp(), sig.returnsPointer(), sig.returnsPointer()) : "void");

            if (isNullable) str = "(" + str + ") | null";
            return str;
        }

        if (isPtr && isNullable) return tp->getName() + " | null";
        return tp->getName();
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

        for (ScriptNamespace* ns : m_namespaces) {
            if (ns->name.size() > 0) {
                l("declare namespace %s {", ns->name.c_str());
                indent++;
            }
            
            bool isFirstType = true;
            bool lastWasPrim = false;
            for (DataType* tp : ns->types) {
                if (tp->isFunction() || tp->getFlags().is_hidden) continue;

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
                if (tp->getFlags().needs_host_construction) l("class %s %s{", tp->getName().c_str(), extends.c_str());
                else l("interface %s %s{", tp->getName().c_str(), extends.c_str());
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

            if (ns->types.size() > 0 && ns->globalFunctions.size() > 0) l("");

            for (Function* f : ns->globalFunctions) {
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

            if (ns->globalFunctions.size() > 0 && ns->globalVars.size() > 0) l("");

            bool isFirst = true;
            for (GlobalVariable& g : ns->globalVars) {
                if (!isFirst) l("");
                isFirst = false;

                u32 sz = g.flags.is_pointer ? 4 : g.type->getSize();
                
                l("/**");
                l(" * @address 0x%X", g.address);
                l(" * @size 0x%X (%d bytes)", sz, sz);
                l(" */");
                l("const %s: %s;", g.name.c_str(), tpName(g.type, g.flags.is_pointer, g.flags.is_nullable).c_str());
            }

            if (ns->name.size() > 0) {
                indent--;
                l("}");
            }
        }

        if (!out.save(m_pathBase + "/scripts/globals.d.ts")) {
            error("Failed to write globals.d.ts file");
            return false;
        }

        return true;
    }
};