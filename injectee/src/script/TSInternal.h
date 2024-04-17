#pragma once
#include <types.h>

#include <v8.h>

namespace t4ext {
    void consoleLog(const v8::FunctionCallbackInfo<v8::Value>& args);
    void consoleWarn(const v8::FunctionCallbackInfo<v8::Value>& args);
    void consoleError(const v8::FunctionCallbackInfo<v8::Value>& args);
    void defineFunc(const v8::FunctionCallbackInfo<v8::Value>& args);
    void requireFunc(const v8::FunctionCallbackInfo<v8::Value>& args);
    void eventPoll(const v8::FunctionCallbackInfo<v8::Value>& args);
    void setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    void setInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    void clearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    void clearInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    void addEngineUpdateListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    void removeEngineUpdateListener(const v8::FunctionCallbackInfo<v8::Value>& args);
};