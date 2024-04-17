#include <script/TSInternal.h>
#include <script/TSHelpers.h>
#include <script/TypeScriptAPI.h>
#include <events/Timeout.h>
#include <events/TypeScriptTimeout.h>
#include <events/Update.h>

#include <Client.h>
#include <utils/Singleton.hpp>

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
        v8Throw(args.GetIsolate(), "The 'require' function is not implemented. Bug stinkee2 about it");
    }
    void eventPoll(const v8::FunctionCallbackInfo<v8::Value>& args) {
        bool doContinue = true;

        TypeScriptAPI* api = (TypeScriptAPI*)args.Data().As<v8::External>()->Value();
        
        utils::Singleton<TimeoutEventType>::Get()->processTimeouts();
        
        if (api->handleEvents()) doContinue = false;
        
        args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), doContinue));
    }
    void setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if (args.Length() == 0) return;
        TypeScriptAPI* api = (TypeScriptAPI*)args.Data().As<v8::External>()->Value();
        v8::Isolate* isolate = args.GetIsolate();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        if (!args[0]->IsFunction()) {
            v8Throw(args.GetIsolate(), "setTimeout expects the first parameter to be a callback function");
            return;
        }

        if (args.Length() >= 2 && !args[1]->IsNumber()) {
            v8Throw(args.GetIsolate(), "setTimeout expects the second parameter to be a duration in milliseconds");
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
            v8Throw(args.GetIsolate(), "setTimeout expects the first parameter to be a callback function");
            return;
        }

        if (args.Length() >= 2 && !args[1]->IsNumber()) {
            v8Throw(args.GetIsolate(), "setTimeout expects the second parameter to be a duration in milliseconds");
            return;
        }

        v8::Local<v8::Function> callback = args[0].As<v8::Function>();
        v8::Local<v8::Number> duration = args[1].As<v8::Number>();

        u32 dur = duration.IsEmpty() ? 0 : u32(duration->Value());
        TypeScriptTimeoutData* data = new TypeScriptTimeoutData(api, isolate, callback, dur, true);
        u32 id = utils::Singleton<TimeoutEventType>::Get()->createTimeout(data);

        args.GetReturnValue().Set(v8::Number::New(isolate, id));
    }
    void clearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if (args.Length() == 0) return;
        TypeScriptAPI* api = (TypeScriptAPI*)args.Data().As<v8::External>()->Value();
        v8::Isolate* isolate = args.GetIsolate();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        if (!args[0]->IsNumber()) {
            v8Throw(args.GetIsolate(), "clearTimeout expects the first parameter to be a number");
            return;
        }

        utils::Singleton<TimeoutEventType>::Get()->removeTimeout(args[0].As<v8::Number>()->Value());
    }
    void clearInterval(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if (args.Length() == 0) return;
        TypeScriptAPI* api = (TypeScriptAPI*)args.Data().As<v8::External>()->Value();
        v8::Isolate* isolate = args.GetIsolate();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        if (!args[0]->IsNumber()) {
            v8Throw(args.GetIsolate(), "clearInterval expects the first parameter to be a number");
            return;
        }

        utils::Singleton<TimeoutEventType>::Get()->removeTimeout(args[0].As<v8::Number>()->Value());
    }
};