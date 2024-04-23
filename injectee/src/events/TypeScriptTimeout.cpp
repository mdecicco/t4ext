#include <events/TypeScriptTimeout.h>
#include <script/TypeScriptAPI.h>

namespace t4ext {
    TypeScriptTimeoutData::TypeScriptTimeoutData(
        TypeScriptAPI* api,
        v8::Isolate* isolate,
        const v8::Local<v8::Function>& callback,
        u32 duration,
        bool doLoop
    ) : ITimeoutData(duration, doLoop), m_callback(isolate, callback){
        m_isolate = isolate;
        m_api = api;
    }

    TypeScriptTimeoutData::~TypeScriptTimeoutData() {
        m_callback.Reset();
    }

    void TypeScriptTimeoutData::execute() {
        v8::HandleScope hs(m_isolate);
        v8::Local<v8::Context> context = m_isolate->GetCurrentContext();
        v8::TryCatch tc(m_isolate);
        m_callback.Get(m_isolate)->Call(context, context->Global(), 0, nullptr);
        if (tc.HasCaught()) m_api->logException(tc);
    }
};