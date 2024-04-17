#pragma once
#include <types.h>
#include <script/IScriptAPI.h>

#include <utils/robin_hood.h>

#include <v8.h>

namespace t4ext {
    void v8Throw(v8::Isolate* isolate, const char* fmt, ...);

    class TypeScriptCallback : public ICallback<void> {
        public:
            TypeScriptCallback(const v8::Local<v8::Function>& func);
            virtual ~TypeScriptCallback();

            v8::Local<v8::Function> get(v8::Isolate* isolate);

        protected:
            v8::Persistent<v8::Function> m_callback;
    };
    
    class TypeScriptAPI : public IScriptAPI {
        public:
            struct ModuleInfo {
                utils::String id;
                utils::Array<utils::String> dependencies;
                v8::Persistent<v8::Function> factory;
                v8::Persistent<v8::Object> exports;
            };

            TypeScriptAPI();
            virtual ~TypeScriptAPI();

            ModuleInfo* getModule(const utils::String& moduleId);
            void defineModule(ModuleInfo* module);
            v8::Isolate* getIsolate();
            v8::Local<v8::Context>& getContext();

            virtual bool initialize();
            virtual bool commitBindings();
            virtual bool shutdown();
            virtual bool executeEntry();

            void logException(const v8::TryCatch& exception);
            bool isReady();
        
        protected:
            bool execute(const v8::Local<v8::String>& source);
            v8::Local<v8::Object> loadModule(ModuleInfo* module);
            void setupContext();
            bool generateDefs();

            bool m_didInitialize;
            std::unique_ptr<v8::Platform> m_platform;
            v8::ArrayBuffer::Allocator* m_arrayBufferAllocator;
            v8::Isolate* m_isolate;
            v8::Local<v8::Context> m_context; // this may need to be persistent
            robin_hood::unordered_map<utils::String, ModuleInfo*> m_modules;
            std::atomic<bool> m_isReady;
    };
};