#include <script/Calling.h>
#include <script/IScriptAPI.h>
#include <script/Conversion.h>
#include <script/TSHelpers.h>

#include <Client.h>
#include <utils/Singleton.hpp>

#include <v8.h>
#include <ffi.h>

#include <types.h>
#include <stdint.h>

namespace t4ext {
    ffi_type* ffiType(DataType* tp) {
        if (!tp) return &ffi_type_void;

        if (tp->isPrimitive()) {
            switch (tp->getPrimitiveType()) {
                case Primitive::pt_char: return &ffi_type_schar;
                case Primitive::pt_i8: return &ffi_type_sint8;
                case Primitive::pt_i16: return &ffi_type_sint16;
                case Primitive::pt_i32: return &ffi_type_sint32;
                case Primitive::pt_i64: return &ffi_type_sint64;
                case Primitive::pt_u8: return &ffi_type_uint8;
                case Primitive::pt_u16: return &ffi_type_uint16;
                case Primitive::pt_u32: return &ffi_type_uint32;
                case Primitive::pt_u64: return &ffi_type_uint64;
                case Primitive::pt_f32: return &ffi_type_float;
                case Primitive::pt_f64: return &ffi_type_double;
                default: break;
            }
        }

        return &ffi_type_pointer;
    }

    void Call_ThisCall(const v8::FunctionCallbackInfo<v8::Value>& callInfo, Function* target) {
        v8::Isolate* isolate = callInfo.GetIsolate();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        Function* f = (Function*)callInfo.Data().As<v8::External>()->Value();
        FunctionSignature& sig = f->getSignature();
        utils::Array<FunctionArgument>& expectedArgs = sig.getArgs();

        void* thisPtr = extractThisPointer(callInfo.This(), isolate);
        if (!thisPtr) {
            v8Throw(
                isolate,
                "Attempted to call function '%s::%s', but failed to obtain pointer to instance",
                sig.getThisTp()->getName().c_str(),
                target->getName().c_str()
            );
            return;
        }

        ffi_cif cif;
        ffi_type* argTypes[16] = { nullptr };
        void* tempValueStorage[16] = { 0 };
        void* argValues[16] = { nullptr };
        
        argTypes[0] = &ffi_type_pointer;
        argValues[0] = &thisPtr;

        utils::Array<ConvAlloc> allocs;
        allocs.reserve(16);

        for (u32 i = 0;i < expectedArgs.size();i++) {
            FunctionArgument& info = expectedArgs[i];

            if (info.flags.is_pointer && !info.flags.is_nullable && (callInfo[i].IsEmpty() || callInfo[i]->IsNull() || callInfo[i]->IsUndefined())) {
                v8Throw(
                    isolate,
                    "Argument '%s' of function '%s::%s' does not accept null values, but null or undefined was provided",
                    info.name.c_str(), sig.getThisTp()->getName().c_str(), target->getName().c_str()
                );

                // delete them all regardless of 'doFree' because the call will not occur
                for (u32 a = 0;a < allocs.size();a++) delete [] allocs[a].alloc;
                return;
            }

            utils::String failurePath = info.name, failureReason;
            if (!convertFromV8(&tempValueStorage[i + 1], callInfo[i], info.type, info.flags.is_pointer, allocs, isolate, failurePath, failureReason)) {
                gClient::Get()->log(
                    "The exception that should follow this message occurred while passing argument '%s' to function '%s::%s'",
                    info.name.c_str(), sig.getThisTp()->getName().c_str(), target->getName().c_str()
                );

                if (failureReason.size() > 0) {
                    v8Throw(isolate, "Failed to convert '%s' from script type to host type with reason: %s", failurePath.c_str(), failureReason.c_str());
                } else {
                    v8Throw(isolate, "Failed to convert '%s' from script type to host type", failurePath.c_str());
                }

                // delete them all regardless of 'doFree' because the call will not occur
                for (u32 a = 0;a < allocs.size();a++) delete [] allocs[a].alloc;
                return;
            }

            argTypes[i + 1] = info.flags.is_pointer ? &ffi_type_pointer : ffiType(info.type);
            argValues[i + 1] = &tempValueStorage[i + 1];
        }

        if (ffi_prep_cif(&cif, FFI_THISCALL, expectedArgs.size() + 1, ffiType(sig.getRetTp()), argTypes) != FFI_OK) {
            v8Throw(isolate, "Failed to prepare FFI for call to function '%s'", f->getName().c_str());
            // delete them all regardless of 'doFree' because the call will not occur
            for (u32 a = 0;a < allocs.size();a++) delete [] allocs[a].alloc;
            return;
        }

        void* returnValue = nullptr;
        ffi_call(&cif, (void(*)())target->getAddress(), &returnValue, argValues);

        if (sig.getRetTp()) {
            v8::Local<v8::Value> ret;
            utils::String failurePath;
            if (!convertToV8(&returnValue, &ret, sig.getRetTp(), sig.returnsPointer(), isolate, failurePath)) {
                gClient::Get()->log(
                    "The exception that should follow this message occurred while processing the return value of function '%s::%s'",
                    sig.getThisTp()->getName().c_str(), target->getName().c_str()
                );

                v8Throw(isolate, "Failed to convert '%s' from host type to script type", failurePath.c_str());

                for (u32 a = 0;a < allocs.size();a++) {
                    if (allocs[a].doFree) delete [] allocs[a].alloc;
                }
                return;
            }

            callInfo.GetReturnValue().Set(ret);
        }
        
        for (u32 a = 0;a < allocs.size();a++) {
            if (allocs[a].doFree) delete [] allocs[a].alloc;
        }
    }

    void Call_CDecl(const v8::FunctionCallbackInfo<v8::Value>& callInfo, Function* target) {
        v8::Isolate* isolate = callInfo.GetIsolate();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        Function* f = (Function*)callInfo.Data().As<v8::External>()->Value();
        FunctionSignature& sig = f->getSignature();
        utils::Array<FunctionArgument>& expectedArgs = sig.getArgs();

        ffi_cif cif;
        ffi_type* argTypes[16] = { nullptr };
        void* tempValueStorage[16] = { 0 };
        void* argValues[16] = { nullptr };

        utils::Array<ConvAlloc> allocs;
        allocs.reserve(16);

        for (u32 i = 0;i < expectedArgs.size();i++) {
            FunctionArgument& info = expectedArgs[i];
            
            if (info.flags.is_pointer && !info.flags.is_nullable && (callInfo[i].IsEmpty() || callInfo[i]->IsNull() || callInfo[i]->IsUndefined())) {
                v8Throw(
                    isolate,
                    "Argument '%s' of global function '%s' does not accept null values, but null or undefined was provided",
                    info.name.c_str(), target->getName().c_str()
                );

                // delete them all regardless of 'doFree' because the call will not occur
                for (u32 a = 0;a < allocs.size();a++) delete [] allocs[a].alloc;
                return;
            }

            utils::String failurePath = info.name, failureReason;
            if (!convertFromV8(&tempValueStorage[i], callInfo[i], info.type, info.flags.is_pointer, allocs, isolate, failurePath, failureReason)) {
                gClient::Get()->log(
                    "The exception that should follow this message occurred while passing argument '%s' to function '%s'",
                    info.name.c_str(), target->getName().c_str()
                );

                if (failureReason.size() > 0) {
                    v8Throw(isolate, "Failed to convert '%s' from script type to host type with reason: %s", failurePath.c_str(), failureReason.c_str());
                } else {
                    v8Throw(isolate, "Failed to convert '%s' from script type to host type", failurePath.c_str());
                }

                // delete them all regardless of 'doFree' because the call will not occur
                for (u32 a = 0;a < allocs.size();a++) delete [] allocs[a].alloc;
                return;
            }

            argTypes[i] = info.flags.is_pointer ? &ffi_type_pointer : ffiType(info.type);
            argValues[i] = &tempValueStorage[i];
        }

        if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, expectedArgs.size(), ffiType(sig.getRetTp()), argTypes) != FFI_OK) {
            v8Throw(isolate, "Failed to prepare FFI for call to function '%s'", f->getName().c_str());
            return;
        }

        void* returnValue = nullptr;
        ffi_call(&cif, (void(*)())target->getAddress(), &returnValue, argValues);

        if (sig.getRetTp()) {
            v8::Local<v8::Value> ret;
            utils::String failurePath;
            if (!convertToV8(&returnValue, &ret, sig.getRetTp(), sig.returnsPointer(), isolate, failurePath)) {
                gClient::Get()->log(
                    "The exception that should follow this message occurred while processing the return value of function '%s'",
                    target->getName().c_str()
                );

                v8Throw(isolate, "Failed to convert '%s' from host type to script type", failurePath.c_str());
                
                for (u32 a = 0;a < allocs.size();a++) {
                    if (allocs[a].doFree) delete [] allocs[a].alloc;
                }
                return;
            }

            callInfo.GetReturnValue().Set(ret);
        }
        
        for (u32 a = 0;a < allocs.size();a++) {
            if (allocs[a].doFree) delete [] allocs[a].alloc;
        }
    }

    void CallHostFunctionFromV8(const v8::FunctionCallbackInfo<v8::Value>& callInfo, Function* target) {
        bool isThiscall = target->getSignature().getThisTp() != nullptr;

        if (target->getSignature().getThisTp()) {
            // assume this until further notice
            Call_ThisCall(callInfo, target);
        } else {
            // assume this until further notice
            Call_CDecl(callInfo, target);
        }
    }

    void HostCallHandler(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        Function* f = (Function*)args.Data().As<v8::External>()->Value();
        FunctionSignature& sig = f->getSignature();
        utils::Array<FunctionArgument>& expectedArgs = sig.getArgs();

        if (expectedArgs.size() != args.Length()) {
            v8Throw(
                isolate,
                "Function '%s' expected %d %s, but %d %s provided",
                f->getName().c_str(),
                expectedArgs.size(),
                expectedArgs.size() == 1 ? "argument" : "arguments",
                args.Length(),
                args.Length() == 1 ? "was" : "were"
            );
            return;
        }

        CallHostFunctionFromV8(args, f);
    }
};