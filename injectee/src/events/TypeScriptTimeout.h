#pragma once
#include <types.h>
#include <events/Timeout.h>
#include <v8.h>

namespace t4ext {
    class TypeScriptAPI;
    class TypeScriptTimeoutData : public ITimeoutData {
        public:
            TypeScriptTimeoutData(TypeScriptAPI* api, v8::Isolate* isolate, const v8::Local<v8::Function>& callback, u32 duration, bool doLoop);
            virtual ~TypeScriptTimeoutData();

            virtual void execute();

        protected:
            v8::Persistent<v8::Function> m_callback;
            v8::Isolate* m_isolate;
            TypeScriptAPI* m_api;
    };
};