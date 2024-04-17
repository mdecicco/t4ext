#include <script/TSHelpers.h>
#include <Client.h>
#include <utils/Singleton.hpp>

#include <stdarg.h>

namespace t4ext {
    v8::Local<v8::String> v8Str(v8::Isolate* isolate, const char* fmt, ...) {
        char str[4096] = { 0 };
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(str, 4096, fmt, ap);
        va_end(ap);

        v8::Local<v8::String> outStr;
        if (!v8::String::NewFromUtf8(isolate, str).ToLocal(&outStr)) {
            gClient::Get()->error("Failed to load string into V8, the string will follow this message for debug purposes");
            gClient::Get()->error(str);
        }

        return outStr;
    }

    v8::Local<v8::String> v8Str(v8::Isolate* isolate, const utils::String& str) {
        v8::Local<v8::String> outStr;
        if (!v8::String::NewFromUtf8(isolate, str.c_str()).ToLocal(&outStr)) {
            gClient::Get()->error("Failed to load string into V8, the string will follow this message for debug purposes");
            gClient::Get()->error(str);
        }

        return outStr;
    }
    
    v8::Local<v8::Function> v8Func(v8::Isolate* isolate, v8Callback cb, v8::Local<v8::Value> data) {
        v8::Local<v8::FunctionTemplate> templ = v8::FunctionTemplate::New(isolate, cb, data);
        v8::Local<v8::Function> result;
        if (!templ->GetFunction(isolate->GetCurrentContext()).ToLocal(&result)) {
            gClient::Get()->error("Failed to load function callback into V8, the function address was 0x%X", cb);
        }

        return result;
    }
    
    void v8SetProp(v8::Local<v8::Object>& obj, const utils::String& name, const v8::Local<v8::Value>& value) {
        if (obj.IsEmpty()) return;
        v8::Isolate* iso = obj->GetIsolate();

        v8::Local<v8::String> key;
        if (!v8::String::NewFromUtf8(iso, name.c_str()).ToLocal(&key)) {
            gClient::Get()->error("Failed to load string into V8, the string will follow this message for debug purposes");
            gClient::Get()->error(name);
            return;
        }

        obj->Set(iso->GetCurrentContext(), v8Str(iso, name), value);
    }
    
    void v8SetPropAccessors(v8::Local<v8::Object>& obj, const utils::String& name, v8Callback getter, v8Callback setter, v8::Local<v8::Value> getterData, v8::Local<v8::Value> setterData) {
        if (obj.IsEmpty() || !getter) return;
        v8::Isolate* iso = obj->GetIsolate();
        
        v8::Local<v8::String> key;
        if (!v8::String::NewFromUtf8(iso, name.c_str()).ToLocal(&key)) {
            gClient::Get()->error("Failed to load string into V8, the string will follow this message for debug purposes");
            gClient::Get()->error(name);
            return;
        }

        obj->SetAccessorProperty(
            key,
            v8Func(iso, getter, getterData),
            setter ? v8Func(iso, setter, setterData) : v8::Local<v8::Function>(),
            setter ? v8::PropertyAttribute::None : v8::PropertyAttribute::ReadOnly
        );
    }

    bool v8GetProp(const v8::Local<v8::Object>& obj, const utils::String& name, v8::Local<v8::Value>* out) {
        if (obj.IsEmpty()) return false;
        v8::Isolate* iso = obj->GetIsolate();

        v8::Local<v8::String> key;
        if (!v8::String::NewFromUtf8(iso, name.c_str()).ToLocal(&key)) {
            gClient::Get()->error("Failed to load string into V8, the string will follow this message for debug purposes");
            gClient::Get()->error(name);
            return false;
        }

        return obj->Get(iso->GetCurrentContext(), key).ToLocal(out);
    }

    void v8Throw(v8::Isolate* isolate, const char* fmt, ...) {
        char msg[1024] = { 0 };
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(msg, 1024, fmt, ap);
        va_end(ap);

        v8::Local<v8::String> msgStr;
        if (!v8::String::NewFromUtf8(isolate, msg).ToLocal(&msgStr)) {
            gClient::Get()->error("Failed to load string into V8 for exception, the exception will follow");
            gClient::Get()->error(msg);
            return;
        }

        isolate->ThrowError(msgStr);
    }
};