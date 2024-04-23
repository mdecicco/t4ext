#include <script/Conversion.h>
#include <script/TypeScriptAPI.h>
#include <script/TSHelpers.h>

#include <Client.h>
#include <utils/Singleton.hpp>

namespace t4ext {
    template <typename T, typename ...Following>
    bool passArg(TypeScriptAPI* api, v8::Isolate* isolate, u32 idx, v8::Local<v8::Value>* out, T arg, Following... rest) {
        constexpr u32 remainingCount = std::tuple_size_v<std::tuple<Following...>>;
        utils::String failurePath;
        bool result = convertToV8(
            &arg,
            &out[idx],
            api->getType<T>(),
            std::is_pointer_v<T> || std::is_reference_v<T>,
            isolate,
            failurePath
        );

        if (!result) {
            if (failurePath.size() > 0) api->error("Failed to pass argument %d to callback (failed at %s)", failurePath.c_str());
            else api->error("Failed to pass argument %d to callback");
            return false;
        }

        if constexpr (remainingCount > 0) {
            return passArg(api, isolate, idx + 1, out, rest...);
        }

        return true;
    }

    template <typename Ret, typename ...Args>
    Ret CallCallback(TypeScriptCallbackData* cb, Args... args) {
        TypeScriptAPI* api = (TypeScriptAPI*)gClient::Get()->getAPI();
        v8::Isolate* isolate = api->getIsolate();
        v8::HandleScope hs(isolate);

        constexpr u32 argc = std::tuple_size_v<std::tuple<Args...>>;
        v8::Local<v8::Value> callArgs[std::tuple_size_v<std::tuple<Args...>> + 1];

        v8::Local<v8::Context> context = api->getContext();

        if constexpr (argc > 0) {
            bool passedArgs = passArg(api, isolate, 0, callArgs, args...);
            if (!passedArgs) throw std::exception("Failed to pass all arguments to callback");
        }

        if constexpr (std::is_same_v<Ret, void>) {
            bool didExcept = false;

            {
                v8::TryCatch tc(isolate);
                tc.SetVerbose(true);
                v8::Local<v8::Function> func = cb->get(isolate);
                v8::MaybeLocal<v8::Value> ret = func->Call(context, context->Global(), argc, callArgs);
                if (tc.HasCaught()) {
                    api->logException(tc);
                    didExcept = true;
                }
            }

            if (didExcept) throw std::exception("Caught exception when calling callback");
        } else {
            Ret result;
            bool didExcept = false;
            bool didGetResult = false;

            {
                v8::TryCatch tc(isolate);
                tc.SetVerbose(true);
                v8::MaybeLocal<v8::Value> ret = cb->get(isolate)->Call(context, context->Global(), argc, callArgs);
                if (tc.HasCaught()) {
                    api->logException(tc);
                    didExcept = true;
                }

                v8::Local<v8::Local> retV;
                if (ret.ToLocal(&retV)) {
                    utils::Array<ConvAlloc> allocs;
                    utils::String failurePath, failureReason;
                    didGetResult = convertFromV8(
                        &result,
                        retV,
                        api->getType<Ret>(),
                        std::is_pointer_v<Ret> || std::is_reference_v<Ret>,
                        allocs,
                        isolate,
                        failurePath,
                        failureReason
                    );

                    if (!didGetResult) {
                        if (failureReason.size() > 0) {
                            api->error("Failed to convert '%s' from script type to host type with reason: %s", failurePath.c_str(), failureReason.c_str());
                        } else {
                            api->error("Failed to convert '%s' from script type to host type", failurePath.c_str());
                        }
                    }
                }
            }

            if (didExcept) throw std::exception("Caught exception when calling callback");
            if (!didGetResult) throw std::exception("Failed to get or convert callback return value");
            return result;
        }
    }
};