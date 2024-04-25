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
    HANDLE tscHandle = nullptr;
    utils::Thread tscThread;

    bool StartCompilerProcess(t4ext::TypeScriptAPI* api) {
        utils::String compilerPath = api->getGameBasePath() + "/scripts/compiler/node.exe";

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

        bool exitMonitor = false;
        bool monitorExited = false;
        utils::Thread logMonitor([readPipe, api, &exitMonitor, &monitorExited](){
            char chBuf[4096] = { 0 };
            utils::String lineBuf;

            utils::IWithLogging tscLog("tsc");
            tscLog.subscribeLogger(api);

            while (!exitMonitor) {
                DWORD dwRead;
                if (ReadFile(readPipe, chBuf, 4096, &dwRead, NULL) == 0 || dwRead == 0) break;

                for (utils::u32 i = 0;i < 4096 && chBuf[i] != 0;i++) {
                    if (chBuf[i] == '\n' || chBuf[i] == '\r') {
                        if (lineBuf.size() == 0) continue;
                        tscLog.log(lineBuf);
                        lineBuf = "";
                        continue;
                    }

                    lineBuf += chBuf[i];
                }

                memset(chBuf, 0, dwRead);
            }

            monitorExited = true;
        });

        utils::String args = "\"" + compilerPath + "\" ./compiler/node_modules/typescript/bin/tsc -p ./tsconfig.json -w";
        utils::String cwd = api->getGameBasePath() + "/scripts";
        if (CreateProcessA(compilerPath.c_str(), (LPSTR)args.c_str(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, cwd.c_str(), &si, &pi) == 0) {
            api->error("Failed to spawn compiler process");
            CloseHandle(readPipe);
            CloseHandle(writePipe);
            return false;
        }

        tscHandle = pi.hProcess;

        api->log("Compiler process spawned");
        WaitForSingleObject(pi.hProcess, INFINITE);

        exitMonitor = true;
        while (!monitorExited) {}

        CloseHandle(writePipe);
        CloseHandle(readPipe);

        DWORD exitCode = 0;
        if (GetExitCodeProcess(pi.hProcess, &exitCode) == 0) {
            api->error("Compilation may have succeeded, but we were unable to get the exit code");
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

        api->log("Compiler process exited with a good status code, nice");

        return true;
    }
};

namespace t4ext {
    TypeScriptCallbackData::TypeScriptCallbackData(const v8::Local<v8::Function>& func) : m_callback(func->GetIsolate(), func) {
    }

    TypeScriptCallbackData::~TypeScriptCallbackData() {
        m_callback.Reset();
    }
    
    v8::Local<v8::Function> TypeScriptCallbackData::get(v8::Isolate* isolate) {
        return m_callback.Get(isolate);
    }

    void buildObjectTemplate(DataType* tp, v8::Isolate* isolate, v8::Local<v8::ObjectTemplate>& templ) {
        v8::HandleScope hs(isolate);

        utils::Array<DataType*>& bases = tp->getBases();
        for (DataType* b : bases) {
            buildObjectTemplate(b, isolate, templ);
        }
        
        utils::Array<DataTypeField>& fields = tp->getFields();
        for (DataTypeField& f : fields) {
            if (f.flags.use_v8_accessors == 0) {
                // will be manually added to the object at conversion time
                continue;
            }

            v8::Local<v8::External> fieldData = v8::External::New(isolate, &f);
            v8::Local<v8::FunctionTemplate> getter = v8::FunctionTemplate::New(isolate, v8Getter, fieldData);
            v8::Local<v8::FunctionTemplate> setter;
            if (f.flags.is_readonly == 0) setter = v8::FunctionTemplate::New(isolate, v8Setter, fieldData);

            templ->SetAccessorProperty(v8Str(isolate, f.name), getter, setter);
        }
        
        utils::Array<Function*>& methods = tp->getMethods();
        for (Function* m : methods) {
            v8::Local<v8::External> methodData = v8::External::New(isolate, m);
            templ->Set(v8Str(isolate, m->getName()), v8::FunctionTemplate::New(isolate, HostCallHandler, methodData));
        }
    }


    TypeScriptAPI::TypeScriptAPI() {
        m_platform = nullptr;
        m_isolate = nullptr;
        m_arrayBufferAllocator = nullptr;
        m_didInitialize = false;
        m_entryModificationTimeOnStartup = 0;
    }

    TypeScriptAPI::~TypeScriptAPI() {
    }

    TypeScriptAPI::ModuleInfo* TypeScriptAPI::getModule(const utils::String& moduleId, bool doReload) {
        v8::HandleScope hs(m_isolate);
        
        auto it = m_modules.find(moduleId);
        if (it != m_modules.end()) {
            ModuleInfo* m = it->second;
            if (doReload) unloadModule(m);
            else return m;
        }
        // didn't think that would work anyway...

        std::filesystem::path mpath(moduleId.c_str());
        if (!mpath.has_extension()) mpath.replace_extension(".js");

        utils::String filename = mpath.filename().string();
        utils::String origin = mpath.remove_filename().string();
        bool didPushOrigin = false;
        if (origin != "./") {
            utils::String path;
            if (m_requireOriginStack.size() > 0) path = m_requireOriginStack.last();

            if (origin[0] == '.' && origin[1] == '/') {
                path += utils::String::View(origin.c_str() + 2, origin.size() - 2);
            } else {
                path += origin;
            }

            m_requireOriginStack.push(path);
            didPushOrigin = true;
        }

        utils::Array<utils::String> tryPaths = {
            filename
        };

        if (m_requireOriginStack.size() > 0) {
            const utils::String& base = m_requireOriginStack.last();
            if (base[0] == '.' && base[1] == '/') {
                utils::String p = base.c_str() + 2;
                tryPaths.push(p + filename);
            } else tryPaths.push(base + filename);
        }

        for (utils::String& p : tryPaths) p = getGameBasePath() + "/scripts/output/" + p;

        utils::String foundAt;
        for (const utils::String& path : tryPaths) {
            struct stat fstat;
            if (stat(path.c_str(), &fstat) == 0) {
                foundAt = path;
                break;
            }
        }

        if (foundAt.size() == 0) {
            error("Could not find module '%s', tried paths:", moduleId.c_str());
            for (const utils::String& path : tryPaths) {
                log("    - %s", path.c_str());
            }
        
            if (didPushOrigin) m_requireOriginStack.pop();
            return nullptr;
        }

        utils::String actualModuleId = "./" + std::filesystem::relative(
            std::filesystem::path(foundAt.c_str()),
            (m_pathBase + "/scripts/output").c_str()
        ).string();

        actualModuleId.replaceAll('\\', '/');

        // Try again with the module path relative to the base directory
        it = m_modules.find(actualModuleId);
        if (it != m_modules.end()) {
            ModuleInfo* m = it->second;
            if (doReload) unloadModule(m);
            else {
                if (didPushOrigin) m_requireOriginStack.pop();
                return m;
            }
        }

        m_requestedModule = actualModuleId;

        // try to load it
        utils::Buffer* buf = utils::Buffer::FromFile(foundAt, true);
        if (!buf) {
            error("Found module '%s' at path '%s', but failed to open it", moduleId.c_str(), foundAt.c_str());
            if (didPushOrigin) m_requireOriginStack.pop();
            return nullptr;
        }

        bool didSucceed = false;
        v8::Local<v8::String> source;
        if (v8::String::NewFromUtf8(m_isolate, (const char*)buf->data()).ToLocal(&source)) {
            didSucceed = execute(source);
        }
        
        delete buf;

        if (didPushOrigin) m_requireOriginStack.pop();

        if (!didSucceed) {
            error("Module '%s' was found at '%s', but there were errors when initializing it", moduleId.c_str(), foundAt.c_str());
            return nullptr;
        }
        
        // see if it's defined _now_
        it = m_modules.find(actualModuleId);
        if (it == m_modules.end()) {
            // It's not, but the execution succeeded so it must not be a true module (no exports, no factory function, etc...)
            // As such, `define` wasn't called, so we have to define it here
            ModuleInfo* m = new ModuleInfo;
            m->id = actualModuleId;

            // reset this, since normally `define` would do it
            m_requestedModule = "";

            m_modules[actualModuleId] = m;

            return m;
        }

        return it->second;
    }

    v8::Local<v8::Object> TypeScriptAPI::loadModule(ModuleInfo* module) {
        v8::EscapableHandleScope hs(m_isolate);
        v8::Local<v8::Context> ctx = m_context.Get(m_isolate);
        
        log("Loading module '%s'", module->id.c_str());

        if (module->exports.IsEmpty()) {
            v8::Local<v8::Object> exports = v8::Object::New(m_isolate);

            if (module->factory.IsEmpty()) {
                warn("Nothing to do for '%s', it's not a module. It did execute though. This is likely not an issue.", module->id.c_str());
                module->exports.Reset(m_isolate, exports);
                return hs.Escape(exports);
            }

            utils::Array<v8::Local<v8::Value>> deps;
            for (u32 i = 0;i < module->dependencies.size();i++) {
                utils::String& depId = module->dependencies[i];
                if (depId == "require") {
                    v8::Local<v8::External> self = v8::External::New(m_isolate, this);
                    deps.push(v8Func(m_isolate, requireFunc, self));
                } else if (depId == "exports") {
                    deps.push(exports);
                } else {
                    utils::String origin = std::filesystem::path(module->id.c_str()).remove_filename().string();
                    m_requireOriginStack.push(origin);
                    ModuleInfo* depMod = getModule(depId);
                    m_requireOriginStack.pop();


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

            {
                v8::TryCatch tc(m_isolate);
                tc.SetVerbose(true);
                
                v8::Local<v8::Function> factory = module->factory.Get(m_isolate);
                factory->Call(ctx, ctx->Global(), deps.size(), deps.data());

                if (tc.HasCaught()) {
                    error("In module '%s'", module->id.c_str());
                    logException(tc);
                }
            }

            module->exports.Reset(m_isolate, exports);
            return hs.Escape(exports);
        }

        return hs.Escape(module->exports.Get(m_isolate));
    }
    
    void TypeScriptAPI::unloadModule(ModuleInfo* m) {
        log("Unloading module '%s'", m->id.c_str());

        if (!m->exports.IsEmpty()) {
            m->exports.Reset();
        }

        if (!m->factory.IsEmpty()) {
            m->factory.Reset();
        }

        for (auto it = m_modules.begin();it != m_modules.end();++it) {
            if (it->second == m) {
                m_modules.erase(it);
                break;
            }
        }

        delete m;
    }

    void TypeScriptAPI::defineModule(ModuleInfo* module) {
        if (module->id.size() == 0) {
            module->id = m_requestedModule;
            m_requestedModule = "";
        }

        m_modules[module->id] = module;
    }
    
    v8::Isolate* TypeScriptAPI::getIsolate() {
        return m_isolate;
    }

    v8::Local<v8::Context> TypeScriptAPI::getContext() {
        return m_context.Get(m_isolate);
    }
    
    DataTypeData* TypeScriptAPI::getTypeData(DataType* type) {
        auto it = m_typeData.find(type);
        if (it == m_typeData.end()) return nullptr;

        return it->second;
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
        cp.constraints.ConfigureDefaultsFromHeapSize(0, 8 * 1024 * 1024);

        m_isolate = v8::Isolate::New(cp);
        if (!m_isolate) {
            error("Failed to create v8 isolate");
            v8::V8::Dispose();
            v8::V8::DisposePlatform();
            delete m_arrayBufferAllocator;
            return false;
        }

        m_isolate->AddNearHeapLimitCallback(+[](void* data, size_t initialHeapSize, size_t currentHeapSize) {
            f32 currentSzMb = f32(currentHeapSize) / 1024.0f / 1024.0f;
            f32 nextSzMb = currentSzMb + 1.0f;
            gClient::Get()->warn("v8 Nearing heap size limit. Growing from %.2f MB to %.2f MB", currentSzMb, nextSzMb);
            return currentHeapSize + (1024 * 1024);
        }, nullptr);

        m_didInitialize = true;
        return true;
    }

    bool TypeScriptAPI::commitBindings() {
        log("Generating globals.d.ts...");
        if (!generateDefs()) return false;

        // get the current modification time of the compiled entry script, if it exists
        struct stat entryStat;
        if (stat((m_pathBase + "/scripts/output/internal/entry.js").c_str(), &entryStat) == 0) {
            m_entryModificationTimeOnStartup = entryStat.st_mtime;
        }

        log("Launching the compiler in watch mode...");
        tsc::tscThread.reset([this](){
            tsc::StartCompilerProcess(this);
        });

        {
            v8::Locker locker(m_isolate);
            v8::Isolate::Scope isolate_scope(m_isolate);
            v8::HandleScope hs(m_isolate);
            v8::Local<v8::Context> context = v8::Context::New(m_isolate);
            m_context.Reset(m_isolate, context);
            setupContext();

            // Create object templates
            for (auto& it : m_typeMap) {
                DataType* tp = it.second;
                if (tp->isPrimitive() || tp->isArray() || tp->isFunction()) continue;
                
                v8::Local<v8::ObjectTemplate> templ = v8::ObjectTemplate::New(m_isolate);
                templ->SetInternalFieldCount(2); // engine object ptr, DataType ptr

                buildObjectTemplate(tp, m_isolate, templ);

                DataTypeData* d = new DataTypeData;
                d->templ.Reset(m_isolate, templ);
                templ->Set(v8Str(m_isolate, "__refresh"), v8::FunctionTemplate::New(m_isolate, ObjectRefresher));
                m_typeData[tp] = d;
            }
        }

        return true;
    }

    bool TypeScriptAPI::shutdown() {
        if (!m_didInitialize) return true;

        if (tsc::tscHandle) {
            log("Terminating compiler process...");
            if (!TerminateProcess(tsc::tscHandle, 0)) {
                error("Failed to terminate the tsc.exe process, sorry about that. You may wanna open task manager and do it yourself");
            }
        }

        for (auto it = m_modules.begin();it != m_modules.end();++it) {
            it->second->exports.Reset();
            it->second->factory.Reset();
            delete it->second;
        }

        for (auto& it : m_typeData) {
            it.second->templ.Reset();
            delete it.second;
        }

        m_eventMutex.lock();
        for (IEventType* tp : m_eventTypes) delete tp;
        for (IEvent* e : m_events) delete e;
        m_eventTypes.clear();
        m_events.clear();
        m_eventMutex.unlock();

        m_context.Reset();
        
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

        // get the current modification time of the compiled entry script, if it exists
        struct stat entryStat;
        log("Waiting for compiler process to compile the entry point...");
        utils::Timer tmr;
        tmr.start();
        do {
            if (stat((m_pathBase + "/scripts/output/internal/entry.js").c_str(), &entryStat) != 0) {
                utils::Thread::Sleep(200);
                continue;
            }

            if (entryStat.st_mtime > m_entryModificationTimeOnStartup) break;
            utils::Thread::Sleep(200);
        } while (tmr.elapsed() < 10.0f);

        if (entryStat.st_mtime == m_entryModificationTimeOnStartup) {
            error("Timed out while waiting for the compiler process... No mods today!");
            return false;
        }

        log("Executing the entrypoint...");

        bool didSucceed = false;

        {
            v8::Locker locker(m_isolate);
            v8::Isolate::Scope isolate_scope(m_isolate);
            v8::HandleScope hs(m_isolate);
            v8::Local<v8::Context> context = m_context.Get(m_isolate);
            v8::Context::Scope cs(context);

            ModuleInfo* mainModule = getModule("./internal/entry");
            if (mainModule) {
                m_isReady = true;
                loadModule(mainModule);

                // there might've been errors, but we're mainly concerned that it actually resolved here
                didSucceed = true;
            }
        }

        return didSucceed;
    }

    bool TypeScriptAPI::handleEvents() {
        bool result = true;

        {
            v8::Locker locker(m_isolate);
            v8::Isolate::Scope isolate_scope(m_isolate);
            v8::HandleScope handle_scope(m_isolate);
            v8::Local<v8::Context> context = m_context.Get(m_isolate);
            v8::Context::Scope cs(context);

            result = IScriptAPI::handleEvents();
        }

        return result;
    }

    void TypeScriptAPI::scriptException(const utils::String& msg) {
        v8Throw(m_isolate, msg.c_str());
    }

    void TypeScriptAPI::logException(const v8::TryCatch& tc) {
        v8::HandleScope hs(m_isolate);
        v8::Local<v8::Context> ctx = m_context.Get(m_isolate);

        utils::String traceStr;
        v8::Local<v8::Value> stackTrace;
        if (tc.StackTrace(ctx).ToLocal(&stackTrace) && stackTrace->IsString() && stackTrace.As<v8::String>()->Length() > 0) {
            traceStr = *v8::String::Utf8Value(m_isolate, stackTrace);
        }

        v8::Local<v8::Message> msg = tc.Message();
        if (!msg.IsEmpty()) {
            // todo: source mapping?

            utils::String msgStr = *v8::String::Utf8Value(m_isolate, msg->Get());
            utils::String fileStr;
            utils::String lineStr;
            i32 lineNum = msg->GetLineNumber(ctx).FromMaybe(-1);
            i32 colStart = msg->GetStartColumn(ctx).FromMaybe(-1);
            i32 colEnd = msg->GetEndColumn(ctx).FromMaybe(-1);

            v8::String::Utf8Value file(m_isolate, msg->GetScriptOrigin().ResourceName());
            fileStr = *file;
            v8::Local<v8::String> lineStrV;
            if (msg->GetSourceLine(ctx).ToLocal(&lineStrV)) {
                lineStr = *v8::String::Utf8Value(m_isolate, lineStrV);
            }

            if (lineStr.size() > 0) {
                utils::String prefix;
                if (lineNum >= 0) utils::String::Format("%d | ", lineNum);
                error(prefix + lineStr);

                if (colStart == -1 && colEnd >= 0) colStart = colEnd;
                if (colEnd == -1 && colStart >= 0) colEnd = colStart;
                if (colStart >= 0) {
                    utils::String idxStr;
                    for (u32 i = 0;i < prefix.size();i++) idxStr += ' ';
                    for (u32 i = 0;i <= colEnd;i++) {
                        if (i >= colStart) idxStr += '^';
                        else idxStr += ' ';
                    }

                    error(idxStr);
                }
            }

            if (traceStr.size() > 0) {
                utils::Array<utils::String> lines = traceStr.split({ "\n", "\r" });
                for (utils::String& tln : lines) error(tln);
            }
        }
    }

    bool TypeScriptAPI::isReady() {
        return m_isReady && !m_shouldTerminate;
    }

    bool TypeScriptAPI::execute(const v8::Local<v8::String>& source) {
        v8::Locker locker(m_isolate);
        v8::Isolate::Scope isolate_scope(m_isolate);
        v8::HandleScope handle_scope(m_isolate);
        v8::Local<v8::Context> context = m_context.Get(m_isolate);
        v8::Context::Scope cs(context);

        bool didSucceed = true;

        v8::TryCatch tc(m_isolate);
        tc.SetVerbose(true);
        v8::Local<v8::Script> script;
        if (!v8::Script::Compile(context, source).ToLocal(&script)) {
            if (tc.HasCaught()) {
                error("Failed to compile script");
                logException(tc);
                didSucceed = false;
            } else {
                error("Failed to compile script, or failed to get compiled script");
            }
            didSucceed = false;
        } else {
            if (!tc.HasCaught()) {
                script->Run(context);
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

    void TypeScriptAPI::setupContext() {
        v8::Locker locker(m_isolate);
        v8::Isolate::Scope isolate_scope(m_isolate);
        v8::HandleScope handle_scope(m_isolate);
        v8::Local<v8::Context> context = m_context.Get(m_isolate);
        v8::Context::Scope cs(context);
        v8::Local<v8::Object> global = context->Global();

        v8SetProp(global, "global", global);
        
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
        t4->SetAccessorProperty(
            v8StrLiteral(m_isolate, "gameDirectory"),
            v8Func(m_isolate, +[](const v8::FunctionCallbackInfo<v8::Value>& args){
                args.GetReturnValue().Set(v8Str(args.GetIsolate(), gClient::Get()->getAPI()->getGameBasePath()));
            }),
            v8::Local<v8::Function>(),
            v8::PropertyAttribute::ReadOnly
        );
        t4->SetAccessorProperty(
            v8StrLiteral(m_isolate, "scriptDirectory"),
            v8Func(m_isolate, +[](const v8::FunctionCallbackInfo<v8::Value>& args){
                args.GetReturnValue().Set(v8Str(args.GetIsolate(), gClient::Get()->getAPI()->getScriptPath()));
            }),
            v8::Local<v8::Function>(),
            v8::PropertyAttribute::ReadOnly
        );

        for (ScriptNamespace* ns : m_namespaces) {
            v8::Local<v8::Object> nsObj;
            if (ns->name.size() == 0) nsObj = context->Global();
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

        if (tp->isArray()) {
            if (isPtr && isNullable) return tp->getElementType()->getName() + "[]" + " | null";
            else return tp->getElementType()->getName() + "[]";
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
            bool noNamespace = true;
            if (ns->name.size() > 0) {
                l("declare namespace %s {", ns->name.c_str());
                indent++;
                noNamespace = false;
            }
            
            bool isFirstType = true;
            bool lastWasPrim = false;
            for (DataType* tp : ns->types) {
                if (tp->isFunction() || tp->isArray() || tp->getFlags().is_hidden) continue;

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

                    str += utils::String("): ") + (sig.getRetTp() ? tpName(sig.getRetTp(), sig.returnsPointer(), sig.getFlags().return_nullable) : "void").c_str() + ";";
                    
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
                utils::String str = utils::String(noNamespace ? "declare function " : "function ") + f->getName();
                str += "(";

                utils::Array<FunctionArgument>& args = sig.getArgs();
                bool isFirst = true;
                for (FunctionArgument& a : args) {
                    if (!isFirst) str += ", ";
                    isFirst = false;

                    str += a.name + ": " + tpName(a.type, a.flags.is_pointer, a.flags.is_nullable);
                }

                str += utils::String("): ") + (sig.getRetTp() ? tpName(sig.getRetTp(), sig.returnsPointer(), sig.getFlags().return_nullable) : "void") + ";";
                
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
                l(noNamespace ? "declare const %s: %s;" :"const %s: %s;", g.name.c_str(), tpName(g.type, g.flags.is_pointer, g.flags.is_nullable).c_str());
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