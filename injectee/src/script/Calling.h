#pragma once
#include <types.h>

#include <v8.h>

namespace t4ext {
    class Function;

    void CallHostFunctionFromV8(const v8::FunctionCallbackInfo<v8::Value>& callInfo, Function* target);
    void HostCallHandler(const v8::FunctionCallbackInfo<v8::Value>& args);
};