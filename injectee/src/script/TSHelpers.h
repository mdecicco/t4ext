#pragma once
#include <types.h>
#include <utils/String.h>

#include <v8.h>

namespace t4ext {
    typedef v8::FunctionCallbackInfo<v8::Value> v8Args;
    typedef void (*v8Callback)(const v8Args&);

    #define v8StrLiteral(isolate, literal) v8::String::NewFromUtf8Literal(isolate, literal)
    v8::Local<v8::String> v8Str(v8::Isolate* isolate, const char* fmt, ...);
    v8::Local<v8::String> v8Str(v8::Isolate* isolate, const utils::String& str);
    v8::Local<v8::Function> v8Func(v8::Isolate* isolate, v8Callback cb, v8::Local<v8::Value> data = v8::Local<v8::Value>());
    template <int N> void v8SetProp(v8::Local<v8::Object>& obj, const char (&name)[N], const v8::Local<v8::Value>& value) {
        if (obj.IsEmpty()) return;
        v8::Isolate* iso = obj->GetIsolate();
        obj->Set(
            iso->GetCurrentContext(),
            v8StrLiteral(iso, name),
            value
        );
    }
    template <int N> void v8SetPropAccessors(v8::Local<v8::Object>& obj, const char (&name)[N], v8Callback getter, v8Callback setter, v8::Local<v8::Value> getterData = v8::Local<v8::Value>(), v8::Local<v8::Value> setterData = v8::Local<v8::Value>()) {
        if (obj.IsEmpty() || !getter) return;
        v8::Isolate* iso = obj->GetIsolate();
        obj->SetAccessorProperty(
            v8StrLiteral(iso, name),
            v8Func(iso, getter, getterData),
            setter ? v8Func(iso, setter, setterData) : v8::Local<v8::Function>(),
            setter ? v8::PropertyAttribute::None : v8::PropertyAttribute::ReadOnly
        );
    }
    template <int N> bool v8GetProp(const v8::Local<v8::Object>& obj, const char (&name)[N], v8::Local<v8::Value>* out) {
        if (obj.IsEmpty()) return false;
        v8::Isolate* iso = obj->GetIsolate();
        return obj->Get(
            iso->GetCurrentContext(),
            v8StrLiteral(iso, name)
        ).ToLocal(out);
    }

    void v8SetProp(v8::Local<v8::Object>& obj, const utils::String& name, const v8::Local<v8::Value>& value);
    void v8SetPropAccessors(v8::Local<v8::Object>& obj, const utils::String& name, v8Callback getter, v8Callback setter, v8::Local<v8::Value> getterData = v8::Local<v8::Value>(), v8::Local<v8::Value> setterData = v8::Local<v8::Value>());
    bool v8GetProp(const v8::Local<v8::Object>& obj, const utils::String& name, v8::Local<v8::Value>* value);
    void v8Throw(v8::Isolate* isolate, const char* fmt, ...);
};