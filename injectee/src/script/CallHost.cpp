#include <script/Calling.h>

#include <v8.h>
#include <types.h>
#include <stdint.h>
#include <script/IScriptAPI.h>
#include <ffi.h>

#include <Client.h>
#include <utils/Singleton.hpp>

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

    bool constructTypeInV8(u64 hostObj, v8::Local<v8::Value>* out, DataType* tp, bool isPtr, v8::Isolate* isolate) {
        if (isPtr) {
            if (!hostObj) {
                *out = v8::Null(isolate);
                return true;
            }

            if (tp->getPrimitiveType() == Primitive::pt_char) {
                v8::Local<v8::String> ret;
                if (!v8::String::NewFromUtf8(isolate, (char*)hostObj).ToLocal(&ret)) {
                    *out = v8::Null(isolate);
                    return true;
                }

                *out = ret;
                return true;
            }
        } else if (tp->isPrimitive()) {
            switch (tp->getPrimitiveType()) {
                case Primitive::pt_char: {
                    v8::Local<v8::String> ret;
                    if (!v8::String::NewFromUtf8(isolate, (char*)&hostObj).ToLocal(&ret)) {
                        isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                            "Failed to convert 'char' to 'string' for some reason, char value was '%d'",
                            hostObj
                        ).c_str()).ToLocalChecked());
                        return false;
                    }

                    *out = ret;
                    return true;
                }
                case Primitive::pt_i8:
                case Primitive::pt_i16:
                case Primitive::pt_i32:
                case Primitive::pt_i64: {
                    *out = v8::Number::New(isolate, f64(hostObj));
                    return true;
                }
                case Primitive::pt_u8:
                case Primitive::pt_u16:
                case Primitive::pt_u32:
                case Primitive::pt_u64:
                case Primitive::pt_enum: {
                    *out = v8::Number::New(isolate, f64(hostObj));
                    return true;
                }
                case Primitive::pt_f32: {
                    *out = v8::Number::New(isolate, *(f32*)&hostObj);
                    return true;
                }
                case Primitive::pt_f64: {
                    *out = v8::Number::New(isolate, *(f64*)&hostObj);
                    return true;
                }
                default: {
                    return false;
                }
            }
        }

        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Local<v8::Object> obj = v8::Object::New(isolate);
        obj->Set(
            isolate->GetCurrentContext(),
            v8::String::NewFromUtf8Literal(isolate, "__this_ptr"),
            v8::Int32::New(isolate, *(i32*)&hostObj)
        );

        utils::Array<DataTypeField>& fields = tp->getFields();
        for (DataTypeField& f : fields) {
            v8::Local<v8::Value> prop;

            u64 val;
            if (f.flags.is_pointer) {
                val = u64(*(void**)(hostObj + f.offset));
            } else {
                if (f.type->getSize() > 8) {
                    isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                        "Type '%s' is larger than 64 bits and is being returned by value, this is unsupported",
                        f.type->getName().c_str()
                    ).c_str()).ToLocalChecked());
                    return false;
                }

                memcpy(&val, (void*)(hostObj + f.offset), f.type->getSize());
            }

            if (!constructTypeInV8(val, &prop, f.type, f.flags.is_pointer, isolate)) {
                return false;
            }

            v8::Local<v8::String> key;
            if (!v8::String::NewFromUtf8(isolate, f.name.c_str()).ToLocal(&key)) {
                isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                    "Failed to create property '%s' of object while converting host type '%s'",
                    f.name.c_str(), tp->getName().c_str()
                ).c_str()).ToLocalChecked());
                return false;
            }

            obj->Set(ctx, key, prop);
        }

        utils::Array<Function*>& methods = tp->getMethods();
        for (Function* m : methods) {
            v8::Local<v8::String> key;
            if (!v8::String::NewFromUtf8(isolate, m->getName().c_str()).ToLocal(&key)) {
                isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                    "Failed to create method '%s' of object while converting host type '%s'",
                    m->getName().c_str(), tp->getName().c_str()
                ).c_str()).ToLocalChecked());
                return false;
            }

            v8::Local<v8::External> data = v8::External::New(isolate, m);
            v8::Local<v8::Function> method = v8::FunctionTemplate::New(isolate, HostCallHandler, data)->GetFunction(ctx).ToLocalChecked();

            obj->Set(ctx, key, method);
        }

        *out = obj;
        return true;
    }

    bool constructTypeFromV8(void* destObj, const v8::Local<v8::Value>& in, DataType* tp, bool expectsPtr, v8::Isolate* isolate) {
        if (tp->isPrimitive() && (!expectsPtr || tp->getPrimitiveType() == Primitive::pt_char)) {
            u64 minVal, maxVal;
            bool isSigned = true;

            switch (tp->getPrimitiveType()) {
                case Primitive::pt_char: {
                    if (in->IsNumber()) {
                        f64 value = in.As<v8::Number>()->Value();
                        if (value < CHAR_MIN) {
                            return false;
                        } else if (value > CHAR_MAX) {
                            return false;
                        }

                        if (expectsPtr) {
                            char* out = new char[2];
                            out[0] = char(value);
                            out[1] = 0;
                            *((char**)destObj) = out;
                        } else {
                            *((char*)destObj) = char(value);
                        }

                        return true;
                    } else if (in->IsString()) {
                        v8::Local<v8::String> s = in.As<v8::String>();

                        if (expectsPtr) {
                            if (s->Length() == 0) *((char**)destObj) = new char(0);
                            else {
                                v8::String::Utf8Value str(isolate, s);
                                char* out = new char[s->Length() + 1];
                                strncpy(out, *str, s->Length());
                                out[s->Length()] = 0;
                                *((char**)destObj) = out;
                            }
                        } else {
                            if (s->Length() != 1) {
                                return false;
                            }

                            v8::String::Utf8Value str(isolate, s);
                            *((char*)destObj) = **str;
                        }

                        return true;
                    }
                    
                    return false;
                }
                case Primitive::pt_i8: { minVal = -INT8_MIN; maxVal = INT8_MAX; break; }
                case Primitive::pt_i16: { minVal = -INT16_MIN; maxVal = INT16_MAX; break; }
                case Primitive::pt_i32: { minVal = -INT32_MIN; maxVal = INT32_MAX; break; }
                case Primitive::pt_i64: { minVal = -INT64_MIN; maxVal = INT64_MAX; break; }
                case Primitive::pt_u8: { minVal = 0; maxVal = UINT8_MAX; isSigned = false; break; }
                case Primitive::pt_u16: { minVal = 0; maxVal = UINT16_MAX; isSigned = false; break; }
                case Primitive::pt_u32: { minVal = 0; maxVal = UINT32_MAX; isSigned = false; break; }
                case Primitive::pt_u64: { minVal = 0; maxVal = UINT64_MAX; isSigned = false; break; }
                case Primitive::pt_enum: {
                    if (!in->IsNumber()) {
                        return false;
                    }

                    f64 value = in.As<v8::Number>()->Value();
                    *(u32*)destObj = u32(value);

                    const utils::Array<DataTypeField>& validValues = tp->getFields();
                    bool found = false;
                    for (u32 i = 0;i < validValues.size();i++) {
                        if (validValues[i].offset == *(u32*)destObj) {
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        return false;
                    }

                    return true;
                }
                case Primitive::pt_f32: {
                    if (!in->IsNumber()) {
                        return false;
                    }
                    
                    f64 value = in.As<v8::Number>()->Value();

                    if (value < f64(FLT_MIN)) {
                        // return false; todo: apparently these checks are invalid???
                    } else if (value > f64(FLT_MAX)) {
                        // return false; todo: apparently these checks are invalid???
                    }

                    *((f32*)destObj) = f32(value);
                    return true;
                }
                case Primitive::pt_f64: {
                    if (!in->IsNumber()) {
                        return false;
                    }

                    *((f64*)destObj) = in.As<v8::Number>()->Value();
                    break;
                }
                default: {
                    return false;
                }
            }

            if (!in->IsNumber()) {
                return false;
            }

            f64 value = in.As<v8::Number>()->Value();

            if (-value > minVal) { // (minVal is actually the positive form of the minimum value)
                return false;
            } else if (value > maxVal) {
                return false;
            }
            
            if (isSigned) *((i64*)destObj) = i64(value);
            else *(u64*)destObj = u64(value);

            return true;
        }

        if (!tp->isPrimitive() && in->IsObject()) {
            v8::Local<v8::Object> obj = in.As<v8::Object>();
            utils::Array<DataTypeField>& fields = tp->getFields();
            v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
            for (DataTypeField& f : fields) {
                v8::Local<v8::String> key;
                if (!v8::String::NewFromUtf8(isolate, f.name.c_str()).ToLocal(&key)) {
                    isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                        "Failed to get property '%s' of object while converting to host type '%s'",
                        f.name.c_str(), tp->getName().c_str()
                    ).c_str()).ToLocalChecked());
                    return false;
                }

                v8::Local<v8::Value> prop;
                if (!obj->Get(ctx, key).ToLocal(&prop)) {
                    isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                        "Failed to get property '%s' of object while converting to host type '%s'",
                        f.name.c_str(), tp->getName().c_str()
                    ).c_str()).ToLocalChecked());
                    return false;
                }

                void* propDest = (u8*)destObj + f.offset;
                if (f.flags.is_pointer) {
                    // todo: make sure we can actually do this...
                    //       also todo: make sure dest gets freed
                    propDest = new u8[f.type->getSize()];
                    *((void**)((u8*)destObj + f.offset)) = propDest;
                }

                if (!constructTypeFromV8(propDest, prop, f.type, f.flags.is_pointer, isolate)) {
                    isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                        "Failed to construct property '%s' of object while converting to host type '%s'",
                        f.name.c_str(), tp->getName().c_str()
                    ).c_str()).ToLocalChecked());
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    u32 extractThisParam(const v8::Local<v8::Object>& maybeThisObj, v8::Isolate* isolate) {
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        if (maybeThisObj.IsEmpty()) return 0;
        if (!maybeThisObj->IsObject()) return 0;
        
        v8::Local<v8::String> ptrKey = v8::String::NewFromUtf8Literal(isolate, "__this_ptr");
        bool hasPtr = false;
        if (!maybeThisObj->HasOwnProperty(ctx, ptrKey).To(&hasPtr) || !hasPtr) return 0;

        v8::Local<v8::Value> thisPtrV;
        if (!maybeThisObj->Get(ctx, ptrKey).ToLocal(&thisPtrV)) return 0;

        i32 thisPtri = thisPtrV.As<v8::Int32>()->Value();
        return *(u32*)&thisPtri;
    }

    bool convertArgPrimitive(
        v8::Local<v8::Value> in, // Value coming in from script call
        u64* out,                // value being passed to the host function
        FunctionArgument& info,  // info about the host function
        Function* func,          // function being called
        v8::Isolate* isolate     // you get it
    ) {
        u64 minVal, maxVal;
        bool isSigned = true;

        switch (info.type->getPrimitiveType()) {
            case Primitive::pt_char: {
                if (in->IsNumber()) {
                    f64 value = in.As<v8::Number>()->Value();
                    if (value < CHAR_MIN) {
                        isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                            "Value provided for argument '%s' of function '%s' is below the minimum possible value for the %s datatype (min: %d)",
                            info.name.c_str(), func->getName(), info.type->getName(), CHAR_MIN
                        ).c_str()).ToLocalChecked());
                        return false;
                    } else if (value > CHAR_MAX) {
                        isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                            "Value provided for argument '%s' of function '%s' is greater than the maximum possible value for the %s datatype (max: %d)",
                            info.name.c_str(), func->getName(), info.type->getName(), CHAR_MAX
                        ).c_str()).ToLocalChecked());
                        return false;
                    }

                    *((char*)out) = char(value);
                } else if (in->IsString()) {
                    v8::Local<v8::String> s = in.As<v8::String>();
                    if (s->Length() != 1) {
                        isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                            "String provided for argument '%s' of function '%s', which expects a 'char', but the provided string does not have length = 1",
                            info.name.c_str(), func->getName()
                        ).c_str()).ToLocalChecked());
                        return false;
                    }

                    v8::String::Utf8Value str(isolate, s);
                    *((char*)out) = **str;
                }
                
                isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                    "Expected number or string for argument '%s' of function '%s'",
                    info.name.c_str(), func->getName()
                ).c_str()).ToLocalChecked());
                return false;
            }
            case Primitive::pt_i8: { minVal = -INT8_MIN; maxVal = INT8_MAX; break; }
            case Primitive::pt_i16: { minVal = -INT16_MIN; maxVal = INT16_MAX; break; }
            case Primitive::pt_i32: { minVal = -INT32_MIN; maxVal = INT32_MAX; break; }
            case Primitive::pt_i64: { minVal = -INT64_MIN; maxVal = INT64_MAX; break; }
            case Primitive::pt_u8: { minVal = 0; maxVal = UINT8_MAX; isSigned = false; break; }
            case Primitive::pt_u16: { minVal = 0; maxVal = UINT16_MAX; isSigned = false; break; }
            case Primitive::pt_u32: { minVal = 0; maxVal = UINT32_MAX; isSigned = false; break; }
            case Primitive::pt_u64: { minVal = 0; maxVal = UINT64_MAX; isSigned = false; break; }
            case Primitive::pt_enum: {
                if (!in->IsNumber()) {
                    isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                        "Expected number for argument '%s' of function '%s'",
                        info.name.c_str(), func->getName()
                    ).c_str()).ToLocalChecked());
                    return false;
                }

                f64 value = in.As<v8::Number>()->Value();
                *out = u32(value);

                const utils::Array<DataTypeField>& validValues = info.type->getFields();
                bool found = false;
                for (u32 i = 0;i < validValues.size();i++) {
                    if (validValues[i].offset == u32(*out)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                        "Value %llu provided for argument '%s' of function '%s' is not valid for enum '%s'",
                        *out, info.name.c_str(), func->getName(), info.type->getName()
                    ).c_str()).ToLocalChecked());
                    return false;
                }

                return true;
            }
            case Primitive::pt_f32: {
                if (!in->IsNumber()) {
                    isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                        "Expected number for argument '%s' of function '%s'",
                        info.name.c_str(), func->getName()
                    ).c_str()).ToLocalChecked());
                    return false;
                }
                
                f64 value = in.As<v8::Number>()->Value();

                if (value < FLT_MIN) {
                    isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                        "Value provided for argument '%s' of function '%s' is below the minimum possible value for the %s datatype (min: %f)",
                        info.name.c_str(), func->getName(), info.type->getName(), FLT_MIN
                    ).c_str()).ToLocalChecked());
                    return false;
                } else if (value > FLT_MAX) {
                    isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                        "Value provided for argument '%s' of function '%s' is greater than the maximum possible value for the %s datatype (max: %f)",
                        info.name.c_str(), func->getName(), info.type->getName(), FLT_MAX
                    ).c_str()).ToLocalChecked());
                    return false;
                }

                *((f32*)out) = f32(value);
                return true;
            }
            case Primitive::pt_f64: {
                if (!in->IsNumber()) {
                    isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                        "Expected number for argument '%s' of function '%s'",
                        info.name.c_str(), func->getName()
                    ).c_str()).ToLocalChecked());
                    return false;
                }

                *((f64*)out) = in.As<v8::Number>()->Value();
                break;
            }
            default: {
                isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                    "Argument '%s' of function '%s' could not be passed",
                    info.name.c_str(), func->getName()
                ).c_str()).ToLocalChecked());
                return false;
            }
        }

        if (!in->IsNumber()) {
            isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                "Expected number for argument '%s' of function '%s'",
                info.name.c_str(), func->getName()
            ).c_str()).ToLocalChecked());
            return false;
        }

        f64 value = in.As<v8::Number>()->Value();

        if (-value > minVal) { // (minVal is actually the positive form of the minimum value)
            isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                "Value provided for argument '%s' of function '%s' is below the minimum possible value for the %s datatype (min: %s%llu)",
                info.name.c_str(), func->getName(), info.type->getName(), minVal == 0 ? "" : "-", minVal
            ).c_str()).ToLocalChecked());
            return false;
        } else if (value > maxVal) {
            isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                "Value provided for argument '%s' of function '%s' is greater than the maximum possible value for the %s datatype (max: %llu)",
                info.name.c_str(), func->getName(), info.type->getName(), maxVal
            ).c_str()).ToLocalChecked());
            return false;
        }
        
        if (isSigned) *((i64*)out) = i64(value);
        else *out = u64(value);

        return true;
    }

    bool convertArg(
        v8::Local<v8::Value> in, // Value coming in from script call
        u64* out,                // value being passed to the host function
        FunctionArgument& info,  // info about the host function
        Function* func,          // function being called
        v8::Isolate* isolate     // you get it
    ) {
        if (info.type->isPrimitive() && !info.flags.is_pointer) return convertArgPrimitive(in, out, info, func, isolate);

        if (info.flags.is_pointer && !info.flags.is_nullable && (in->IsNull() || in->IsUndefined())) {
            isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                "Argument '%s' of function '%s' does not accept null values, but null or undefined was provided",
                info.name.c_str(), func->getName()
            ).c_str()).ToLocalChecked());
            return false;
        }

        // todo:
        //  - this function should get a reference to an array of dynamically allocated memory
        //    blocks that this function will need to generate in order to store the constructed
        //    types (so that they can be deallocated after the call completes)
        //  - If function expects a pointer (it should almost always expect that for non-primitives)
        //    but the object provided from v8 doesn't have a '__this_ptr' property then we have to
        //    check if we even _can_ construct the object manually in a way that won't cause the
        //    game to crash due to receiving info it doesn't expect
        //  - If the function expects a pointer and the object provided by v8 _does_ have a '__this_ptr',
        //    then just get that pointer and pass that and don't bother trying to construct anything

        if (info.flags.is_pointer) {
            if (in->IsObject()) {
                v8::Local<v8::Object> obj = in.As<v8::Object>();
                u32 thisArg = extractThisParam(obj, isolate);
                if (thisArg != 0) {
                    *out = thisArg;
                    return true;
                }
            }

            if (info.type->getPrimitiveType() == Primitive::pt_char) {
                if (!constructTypeFromV8(out, in, info.type, info.flags.is_pointer, isolate)) {
                    return false;
                }
                
                return true;
            }

            // todo: make sure we can actually do this...
            //       also todo: make sure dest gets freed
            u8* dest = new u8[info.type->getSize()];
            *out = (u64)dest;
            if (!constructTypeFromV8(dest, in, info.type, info.flags.is_pointer, isolate)) {
                return false;
            }
            
            return true;
        }

        return false;
    }

    void Call_ThisCall(const v8::FunctionCallbackInfo<v8::Value>& callInfo, Function* target) {
        v8::Isolate* isolate = callInfo.GetIsolate();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        Function* f = (Function*)callInfo.Data().As<v8::External>()->Value();
        FunctionSignature& sig = f->getSignature();
        utils::Array<FunctionArgument>& expectedArgs = sig.getArgs();

        u32 thisPtr = 0;
        
        if (callInfo.This().IsEmpty()) {
            isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                "Calling method '%s' of type '%s', but 'this' is empty",
                f->getName().c_str(), sig.getThisTp()->getName().c_str()
            ).c_str()).ToLocalChecked());
            return;
        }
        
        if (!callInfo.This()->IsObject()) {
            isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                "Calling method '%s' of type '%s', but 'this' is not an object",
                f->getName().c_str(), sig.getThisTp()->getName().c_str()
            ).c_str()).ToLocalChecked());
            return;
        }
        
        v8::Local<v8::String> ptrKey = v8::String::NewFromUtf8Literal(isolate, "__this_ptr");
        bool hasPtr = false;
        if (!callInfo.This()->HasOwnProperty(ctx, ptrKey).To(&hasPtr) || !hasPtr) {
            isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                "Calling method '%s' of type '%s', but 'this' object doesn't contain the internal object pointer (or we failed to obtain it)",
                f->getName().c_str(), sig.getThisTp()->getName().c_str()
            ).c_str()).ToLocalChecked());
            return;
        }

        v8::Local<v8::Value> thisPtrV;
        if (!callInfo.This()->Get(ctx, ptrKey).ToLocal(&thisPtrV)) {
            isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                "Calling method '%s' of type '%s', but failed to obtain the 'this' pointer",
                f->getName().c_str(), sig.getThisTp()->getName().c_str()
            ).c_str()).ToLocalChecked());
            return;
        }

        // whew...
        i32 thisPtri = thisPtrV.As<v8::Int32>()->Value();
        thisPtr = *(u32*)&thisPtri; // not sure if the weird casting is necessary but whatever

        ffi_cif cif;
        ffi_type* argTypes[16] = { nullptr };
        u64 tempValueStorage[16] = { 0 };
        void* argValues[16] = { nullptr };
        
        argTypes[0] = &ffi_type_pointer;
        argValues[0] = &thisPtr;

        for (u32 i = 0;i < expectedArgs.size();i++) {
            argTypes[i + 1] = expectedArgs[i].flags.is_pointer ? &ffi_type_pointer : ffiType(expectedArgs[i].type);

            if (!convertArg(callInfo[i], &tempValueStorage[i + 1], expectedArgs[i], target, isolate)) {
                // v8 exception already thrown, just return
                return;
            }

            argValues[i + 1] = &tempValueStorage[i + 1];
        }

        if (ffi_prep_cif(&cif, FFI_THISCALL, expectedArgs.size() + 1, ffiType(sig.getRetTp()), argTypes) != FFI_OK) {
            isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                "Failed to prepare FFI for call to function '%s'",
                f->getName().c_str()
            ).c_str()).ToLocalChecked());
            return;
        }

        u64 returnValue = 0;
        ffi_call(&cif, (void(*)())target->getAddress(), &returnValue, argValues);

        if (sig.getRetTp()) {
            v8::Local<v8::Value> ret;
            if (!constructTypeInV8(returnValue, &ret, sig.getRetTp(), sig.returnsPointer(), isolate)) {
                return;
            }

            callInfo.GetReturnValue().Set(ret);
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
        u64 tempValueStorage[16] = { 0 };
        void* argValues[16] = { nullptr };

        for (u32 i = 0;i < expectedArgs.size();i++) {
            argTypes[i] = expectedArgs[i].flags.is_pointer ? &ffi_type_pointer : ffiType(expectedArgs[i].type);

            if (!convertArg(callInfo[i], &tempValueStorage[i], expectedArgs[i], target, isolate)) {
                // v8 exception already thrown, just return
                return;
            }

            argValues[i] = &tempValueStorage[i];
        }

        if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, expectedArgs.size(), ffiType(sig.getRetTp()), argTypes) != FFI_OK) {
            isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                "Failed to prepare FFI for call to function '%s'",
                f->getName()
            ).c_str()).ToLocalChecked());
            return;
        }

        u64 returnValue = 0;
        ffi_call(&cif, (void(*)())target->getAddress(), &returnValue, argValues);

        if (sig.getRetTp()) {
            v8::Local<v8::Value> ret;
            if (!constructTypeInV8(returnValue, &ret, sig.getRetTp(), sig.returnsPointer(), isolate)) {
                return;
            }

            callInfo.GetReturnValue().Set(ret);
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
            isolate->ThrowError(v8::String::NewFromUtf8(isolate, utils::String::Format(
                "Function '%s' expected %d %s, but %d %s provided",
                f->getName().c_str(),
                expectedArgs.size(),
                expectedArgs.size() == 1 ? "argument" : "arguments",
                args.Length(),
                args.Length() == 1 ? "was" : "were"
            ).c_str()).ToLocalChecked());
            return;
        }

        CallHostFunctionFromV8(args, f);
    }
};