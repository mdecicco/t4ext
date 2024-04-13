#pragma once
#include <types.h>
#include <script/IScriptAPI.h>

#include <utils/robin_hood.h>

#include <v8.h>

namespace t4ext {
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

            virtual bool initialize();
            virtual bool commitBindings();
            virtual bool shutdown();
            virtual bool executeEntry();

            bool executeScriptFile(const char* path);
            bool executeScriptMemory(const char* script);
            void logException(const v8::Local<v8::Context>& context, const v8::TryCatch& exception);
        
        protected:
            bool execute(const v8::Local<v8::Context>& context, const v8::Local<v8::String>& source);
            v8::Local<v8::Object> loadModule(const v8::Local<v8::Context>& context, ModuleInfo* module);
            void setupContext(const v8::Local<v8::Context>& context);
            bool generateDefs();

            bool m_didInitialize;
            std::unique_ptr<v8::Platform> m_platform;
            v8::ArrayBuffer::Allocator* m_arrayBufferAllocator;
            v8::Isolate* m_isolate;
            robin_hood::unordered_map<utils::String, ModuleInfo*> m_modules;
    };
};