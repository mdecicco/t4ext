#include <script/Conversion.h>
#include <script/TypeScriptAPI.h>
#include <script/IScriptAPI.hpp>
#include <script/TSHelpers.h>
#include <script/Calling.h>

#include <Client.h>
#include <utils/Singleton.hpp>

namespace t4ext {
    template <typename T>
    inline bool setPrimOut(void* hostObj, bool isPtr, v8::Local<v8::Value>* out, v8::Isolate* isolate) {
        // Checking that *(T**)hostObj is not null is the responsibility of the caller
        *out = v8::Number::New(isolate, f64(isPtr ? **(T**)hostObj : *(T*)hostObj));
        return true;
    }

    void collectFields(DataType* tp, utils::Array<DataTypeField>& fields) {
        utils::Array<DataType*>& bases = tp->getBases();
        for (DataType* b : bases) collectFields(b, fields);
        
        utils::Array<DataTypeField>& ownFields = tp->getFields();
        for (DataTypeField& f : ownFields) {
            fields.push(f);
        }
    }

    bool convertToV8(
        void* hostObj,
        v8::Local<v8::Value>* out,
        DataType* tp,
        bool isPtr,
        v8::Isolate* isolate,
        utils::String& failurePath,
        DataTypeField* selfField
    ) {
        if (!hostObj || (isPtr && !*(void**)hostObj)) {
            *out = v8::Null(isolate);
            return true;
        }

        if (tp->isFunction()) {
            // unsupported behavior
            return false;
        }

        if (tp->isArray()) {
            v8::EscapableHandleScope hs(isolate);
            // should mimic utils::Array
            struct _arr {
                u32 size;
                u32 capacity;
                u8* data;
            };
            _arr* arrIn = isPtr ? *(_arr**)hostObj : (_arr*)hostObj;
            utils::Array<v8::Local<v8::Value>> elems;
            
            DataType* elemTp = tp->getElementType();
            bool elemTpPtr = tp->areElementsPointers();
            u32 elemSz = elemTpPtr ? sizeof(void*) : tp->getElementType()->getSize();

            for (u32 i = 0;i < arrIn->size;i++) {
                v8::Local<v8::Value> elem;
                if (!convertToV8(arrIn->data + (i * elemSz), &elem, elemTp, elemTpPtr, isolate, failurePath, nullptr)) {
                    const char* op = isPtr ? "->" : ".";
                    if (selfField) failurePath = op + utils::String::Format("[%d]", i) + failurePath;
                    else failurePath = utils::String::Format("(%s @ 0x%X)%s%s", tp->getName().c_str(), hostObj, op, failurePath.c_str());
                    
                    return false;
                }
                elems.push(elem);
            }

            *out = hs.Escape(v8::Array::New(isolate, elems.data(), elems.size()));
            return true;
        }

        if (tp->isPrimitive()) {
            switch (tp->getPrimitiveType()) {
                case Primitive::pt_char: {
                    if (isPtr) {
                        *out = v8Str(isolate, *(char**)hostObj);
                    } else {
                        char tmp[2] = { *(char*)hostObj, 0 };
                        *out = v8Str(isolate, tmp);
                    }
                    return true;
                }
                case Primitive::pt_i8: return setPrimOut<i8>(hostObj, isPtr, out, isolate);
                case Primitive::pt_i16: return setPrimOut<i16>(hostObj, isPtr, out, isolate);
                case Primitive::pt_i32: return setPrimOut<i32>(hostObj, isPtr, out, isolate);
                case Primitive::pt_i64: return setPrimOut<i64>(hostObj, isPtr, out, isolate);
                case Primitive::pt_u8: return setPrimOut<u8>(hostObj, isPtr, out, isolate);
                case Primitive::pt_u16: return setPrimOut<u16>(hostObj, isPtr, out, isolate);
                case Primitive::pt_u32: return setPrimOut<u32>(hostObj, isPtr, out, isolate);
                case Primitive::pt_u64: return setPrimOut<u64>(hostObj, isPtr, out, isolate);
                case Primitive::pt_enum: {
                    switch (tp->getSize()) {
                        case sizeof(u8): return setPrimOut<u8>(hostObj, isPtr, out, isolate);
                        case sizeof(u16): return setPrimOut<u16>(hostObj, isPtr, out, isolate);
                        case sizeof(u32): return setPrimOut<u32>(hostObj, isPtr, out, isolate);
                        case sizeof(u64): return setPrimOut<u64>(hostObj, isPtr, out, isolate);
                    }
                    return false;
                }
                case Primitive::pt_f32: return setPrimOut<f32>(hostObj, isPtr, out, isolate);
                case Primitive::pt_f64: return setPrimOut<f64>(hostObj, isPtr, out, isolate);
                default: return false;
            }
        }

        if (isPtr) hostObj = *(void**)hostObj;
        // By this point, hostObj points to the actual memory address
        // of an instance of the type referred to by DataType.

        v8::EscapableHandleScope hs(isolate);
        
        TypeScriptAPI* api = (TypeScriptAPI*)gClient::Get()->getAPI();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Local<v8::ObjectTemplate> templ = api->getTypeData(tp)->templ.Get(isolate);
        v8::Local<v8::Object> obj;
        if (!templ->NewInstance(isolate->GetCurrentContext()).ToLocal(&obj)) return false;

        //v8::Local<v8::Object> obj = v8::Object::New(isolate);
        specifyThisPointer(obj, hostObj, tp, isolate);
        
        if (tp->getBases().size() == 0) {
            // If there's no bases, just do it here to avoid creating an array
            utils::Array<DataTypeField>& fields = tp->getFields();

            for (DataTypeField& f : fields) {
                if (f.flags.use_v8_accessors == 1) {
                    // was exposed via getter/setter in the template
                    continue;
                }

                v8::Local<v8::Value> prop;

                if (!convertToV8((u8*)hostObj + f.offset, &prop, f.type, f.flags.is_pointer, isolate, failurePath, &f)) {
                    const char* op = isPtr ? "->" : ".";
                    if (selfField) failurePath = op + f.name + failurePath;
                    else failurePath = utils::String::Format("(%s @ 0x%X)%s%s", tp->getName().c_str(), hostObj, op, failurePath.c_str());
                    
                    return false;
                }

                v8SetProp(obj, f.name, prop);
            }
        } else {
            utils::Array<DataTypeField> fields;
            collectFields(tp, fields);

            for (DataTypeField& f : fields) {
                if (f.flags.use_v8_accessors == 1) {
                    // was exposed via getter/setter in the template
                    continue;
                }

                v8::Local<v8::Value> prop;

                if (!convertToV8((u8*)hostObj + f.offset, &prop, f.type, f.flags.is_pointer, isolate, failurePath, &f)) {
                    const char* op = isPtr ? "->" : ".";
                    if (selfField) failurePath = op + f.name + failurePath;
                    else failurePath = utils::String::Format("(%s @ 0x%X)%s%s", tp->getName().c_str(), hostObj, op, failurePath.c_str());
                    
                    return false;
                }

                v8SetProp(obj, f.name, prop);
            }
        }

        *out = hs.Escape(obj);
        return true;
    }

    bool convertFromV8(
        void** destObj,
        const v8::Local<v8::Value>& in,
        DataType* tp,
        bool expectsPtr,
        utils::Array<ConvAlloc>& outAllocs,
        v8::Isolate* isolate,
        utils::String& failurePath,
        utils::String& failureReason,
        DataTypeField* selfField
    ) {
        if (in.IsEmpty() || in->IsNull() || in->IsUndefined()) {
            if (expectsPtr) {
                // valid
                *destObj = nullptr;
                return true;
            }

            failureReason = "null | undefined value provided when a non-null value was required";
            return false;
        }
        
        if (!expectsPtr && tp->getSize() > sizeof(void*)) {
            failureReason = "Expected value type is wider than a void*, this is currently unsupported behavior";
            return false;
        }

        v8::HandleScope hs(isolate);

        if (tp->isPrimitive()) {
            u64 minVal, maxVal;
            bool isSigned = true;

            switch (tp->getPrimitiveType()) {
                case Primitive::pt_char: {
                    if (in->IsNumber()) {
                        f64 value = in.As<v8::Number>()->Value();
                        if (value < CHAR_MIN || value > CHAR_MAX) {
                            failureReason = utils::String::Format("Value %d is out of range for type 'char'", i32(value));
                            return false;
                        }

                        if (expectsPtr) {
                            char* out = (char*)new u8[2];
                            out[0] = char(value);
                            out[1] = 0;
                            *destObj = out;

                            outAllocs.push({ (u8*)out, true });
                        } else {
                            *((char*)destObj) = char(value);
                        }

                        return true;
                    } else if (in->IsString()) {
                        v8::Local<v8::String> s = in.As<v8::String>();

                        if (expectsPtr) {
                            if (s->Length() == 0) {
                                u8* str = new u8[1];
                                str[0] = 0;
                                *destObj = str;
                                outAllocs.push({ str, true });
                            } else {
                                v8::String::Utf8Value str(isolate, s);
                                char* out = (char*)new u8[s->Length() + 1];
                                strncpy(out, *str, s->Length());
                                out[s->Length()] = 0;
                                *destObj = out;

                                outAllocs.push({ (u8*)out, true });
                            }
                        } else {
                            if (s->Length() != 1) {
                                failureReason = utils::String::Format("Expected string of length 1 for 'char' type, but got string of length %d", s->Length());
                                return false;
                            }

                            v8::String::Utf8Value str(isolate, s);
                            *((char*)destObj) = (*str)[0];
                        }

                        return true;
                    }
                    
                    v8::String::Utf8Value typeStr(isolate, in->TypeOf(isolate));
                    failureReason = utils::String::Format("Expected string of length 1, or a number for 'char' type, but got %s", *typeStr);
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
                        v8::String::Utf8Value typeStr(isolate, in->TypeOf(isolate));
                        failureReason = utils::String::Format("Expected numerical enum value but %s was provided", *typeStr);
                        return false;
                    }

                    i64 provided = i64(in.As<v8::Number>()->Value());
                    const utils::Array<DataTypeField>& validValues = tp->getFields();
                    bool found = false;
                    for (u32 i = 0;i < validValues.size();i++) {
                        if (provided == i64(validValues[i].offset)) {
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        failureReason = utils::String::Format("Provided value %d does not match any of the valid values for enum type '%s'", provided, tp->getName().c_str());
                        return false;
                    }

                    u64 enumValue = u64(provided);
                    void* destPtr = destObj;

                    if (expectsPtr) {
                        destPtr = new u8[tp->getSize()];
                        *destObj = destPtr;
                        outAllocs.push({ (u8*)destPtr, true });
                    }

                    switch (tp->getSize()) {
                        case sizeof(u64): *((u64*)destPtr) = u64(enumValue);
                        case sizeof(u32): *((u32*)destPtr) = u32(enumValue);
                        case sizeof(u16): *((u16*)destPtr) = u16(enumValue);
                        case sizeof(u8): *((u8*)destPtr) = u8(enumValue);
                    }

                    return true;
                }
                case Primitive::pt_f32: {
                    if (!in->IsNumber()) {
                        v8::String::Utf8Value typeStr(isolate, in->TypeOf(isolate));
                        failureReason = utils::String::Format("Expected numerical value but %s was provided", *typeStr);
                        return false;
                    }
                    
                    f64 value = in.As<v8::Number>()->Value();

                    // todo: validate
                    // for some reason comparisons with FLT_MIN and FLT_MAX gives false positives
                    // even though f64 is wide enough to check against those values

                    if (expectsPtr) {
                        *destObj = new u8[sizeof(f32)];
                        outAllocs.push({ (u8*)*destObj, true });
                        *(*(f32**)destObj) = f32(value);
                    } else {
                        *((f32*)destObj) = f32(value);
                    }

                    return true;
                }
                case Primitive::pt_f64: {
                    if (!in->IsNumber()) {
                        v8::String::Utf8Value typeStr(isolate, in->TypeOf(isolate));
                        failureReason = utils::String::Format("Expected numerical value but %s was provided", *typeStr);
                        return false;
                    }

                    f64 value = in.As<v8::Number>()->Value();

                    if (expectsPtr) {
                        *destObj = new u8[sizeof(f64)];
                        outAllocs.push({ (u8*)*destObj, true });
                        *(*(f64**)destObj) = value;
                    } else {
                        *((f64*)destObj) = value;
                    }

                    return true;
                }
                default: return false;
            }

            if (!in->IsNumber() && !in->IsBoolean()) {
                v8::String::Utf8Value typeStr(isolate, in->TypeOf(isolate));
                failureReason = utils::String::Format("Expected numerical value but %s was provided", *typeStr);
                return false;
            }

            f64 value = in->IsNumber() ? f64(in.As<v8::Number>()->Value()) : f64(in.As<v8::Boolean>()->Value() ? 1.0 : 0.0);

            if (-value > minVal || value > maxVal) { // (minVal is actually the positive form of the minimum value)
                if (isSigned) failureReason = utils::String::Format("Value %lli is out of range for type '%s'", i64(value), tp->getName().c_str());
                else failureReason = utils::String::Format("Value %llu is out of range for type '%s'", u64(value), tp->getName().c_str());
                return false;
            }

            void* destPtr = destObj;
            if (expectsPtr) {
                destPtr = new u8[tp->getSize()];
                *destObj = destPtr;
                outAllocs.push({ (u8*)destPtr, true });
            }
            
            if (isSigned) {
                switch (tp->getSize()) {
                    case sizeof(u64): *((u64*)destPtr) = u64(value);
                    case sizeof(u32): *((u32*)destPtr) = u32(value);
                    case sizeof(u16): *((u16*)destPtr) = u16(value);
                    case sizeof(u8): *((u8*)destPtr) = u8(value);
                }
            } else {
                switch (tp->getSize()) {
                    case sizeof(i64): *((i64*)destPtr) = i64(value);
                    case sizeof(i32): *((i32*)destPtr) = i32(value);
                    case sizeof(i16): *((i16*)destPtr) = i16(value);
                    case sizeof(i8): *((i8*)destPtr) = i8(value);
                }
            }

            return true;
        }

        if (tp->isFunction()) {
            if (!in->IsFunction()) {
                v8::String::Utf8Value typeStr(isolate, in->TypeOf(isolate));
                failureReason = utils::String::Format("Expected function value but %s was provided", *typeStr);
                return false;
            }

            // todo: callback / callback data allocators???
            v8::Local<v8::Function> func = in.As<v8::Function>();
            *destObj = new Callback<void>(new TypeScriptCallbackData(func));

            return true;
        }

        if (tp->isArray()) {
            if (!in->IsArray()) {
                v8::String::Utf8Value typeStr(isolate, in->TypeOf(isolate));
                failureReason = utils::String::Format("Expected array value but %s was provided", *typeStr);
                return false;
            }

            v8::Local<v8::Array> arr = in.As<v8::Array>();
            DataType* elemTp = tp->getElementType();
            bool elemTpPtr = tp->areElementsPointers();
            u32 elemSz = elemTpPtr ? sizeof(void*) : tp->getElementType()->getSize();
            u32 length = arr->Length();

            // must mimic utils::Array
            struct _arr {
                u32 size;
                u32 capacity;
                u8* data;
            };
            _arr* outArr = (_arr*)new u8[sizeof(_arr)];
            outAllocs.push({ (u8*)outArr, true });
            outArr->size = length;
            outArr->capacity = length;
            outArr->data = nullptr;

            if (length > 0) {
                outArr->data = new u8[length * elemSz];
                outAllocs.push({ outArr->data, true });

                v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
                for (u32 i = 0;i < length;i++) {
                    void* fieldPtr = outArr->data + (i * elemSz);
                    v8::Local<v8::Value> elem;
                    if (!arr->Get(ctx, i).ToLocal(&elem)) {
                        failureReason = utils::String::Format("Failed to obtain value at array index %d with element type '%s'", i, elemTp->getName().c_str());
                        return false;
                    }

                    if (!convertFromV8((void**)fieldPtr, elem, elemTp, elemTpPtr, outAllocs, isolate, failurePath, failureReason, selfField)) {
                        if (selfField) failurePath = utils::String::Format("[%d]", i) + failurePath;
                        else failurePath = utils::String::Format("value.%s", failurePath.c_str());
                        return false;
                    }
                }
            }

            *destObj = outArr;
            return true;
        }

        if (!in->IsObject()) {
            v8::String::Utf8Value typeStr(isolate, in->TypeOf(isolate));
            failureReason = utils::String::Format("Expected object type, but got '%s'", *typeStr);
            return false;
        }

        void* existingHostObj = extractThisPointer(in.As<v8::Object>(), isolate);
        if (existingHostObj) {
            if (expectsPtr) {
                // cool, easy
                *destObj = existingHostObj;
                return true;
            }

            // we have no idea whether or not the host object is trivially
            // copyable like this
            //
            // It will likely become necessary to bind copy constructors to
            // data types just for this
            //
            // note: this function checked earlier if the type can fit within a void*, this memcpy
            //       shouldn't corrupt anything
            memcpy(destObj, existingHostObj, tp->getSize());
            return true;
        }
        
        void* objStorage = destObj;
        if (expectsPtr) {
            objStorage = new u8[tp->getSize()];
            outAllocs.push({ (u8*)objStorage, true });
            *destObj = objStorage;
        }

        if (tp->getFlags().needs_host_construction) {
            failureReason = utils::String::Format(
                "Type '%s' /must/ be constructed natively by the game itself, but the object provided was constructed manually by the script",
                tp->getName().c_str()
            );
            return false;
        }

        v8::Local<v8::Object> obj = in.As<v8::Object>();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        if (tp->getBases().size() == 0) {
            // If there's no bases, just do it here to avoid creating an array
            utils::Array<DataTypeField>& fields = tp->getFields();
            for (DataTypeField& f : fields) {
                v8::Local<v8::Value> prop;
                if (!v8GetProp(obj, f.name, &prop)) {
                    failureReason = utils::String::Format("Expected an object similar to type '%s', but property '%s' was missing", tp->getName().c_str(), f.name.c_str());
                    return false;
                }

                void* fieldPtr = (u8*)objStorage + f.offset;
                if (!convertFromV8((void**)fieldPtr, prop, f.type, f.flags.is_pointer, outAllocs, isolate, failurePath, failureReason, selfField)) {
                    if (selfField) failurePath = "." + f.name + failurePath;
                    else failurePath = utils::String::Format("value.%s", failurePath.c_str());
                    return false;
                }
            }
        } else {
            utils::Array<DataTypeField> fields;
            collectFields(tp, fields);

            for (DataTypeField& f : fields) {
                v8::Local<v8::Value> prop;
                if (!v8GetProp(obj, f.name, &prop)) {
                    failureReason = utils::String::Format("Expected an object similar to type '%s', but property '%s' was missing", tp->getName().c_str(), f.name.c_str());
                    return false;
                }

                void* fieldPtr = (u8*)objStorage + f.offset;
                if (!convertFromV8((void**)fieldPtr, prop, f.type, f.flags.is_pointer, outAllocs, isolate, failurePath, failureReason, selfField)) {
                    if (selfField) failurePath = "." + f.name + failurePath;
                    else failurePath = utils::String::Format("value.%s", failurePath.c_str());
                    return false;
                }
            }
        }

        return true;
    }
    
    void specifyThisPointer(v8::Local<v8::Object>& onObject, void* thisPointer, DataType* type, v8::Isolate* isolate) {
        onObject->SetInternalField(0, v8::External::New(isolate, thisPointer));
        onObject->SetInternalField(1, v8::External::New(isolate, type));
        v8SetProp(onObject, "__obj_id", v8::Uint32::New(isolate, (u32)thisPointer));
    }

    void* extractThisPointer(const v8::Local<v8::Object>& maybeThisObj, v8::Isolate* isolate, DataType** outDataType) {
        if (maybeThisObj.IsEmpty()) return nullptr;
        if (!maybeThisObj->IsObject()) return nullptr;
        if (maybeThisObj->InternalFieldCount() != 2) return nullptr;

        void* result = nullptr;
        {
            v8::HandleScope hs(isolate);
            v8::Local<v8::External> ptr = maybeThisObj->GetInternalField(0).As<v8::Value>().As<v8::External>();
            if (ptr.IsEmpty() || !ptr->IsExternal()) return nullptr;

            if (outDataType) {
                v8::Local<v8::Value> typePtr = maybeThisObj->GetInternalField(1).As<v8::Value>().As<v8::External>();
                if (typePtr.IsEmpty() || !typePtr->IsExternal()) return nullptr;
                *outDataType = (DataType*)typePtr.As<v8::External>()->Value();
            }

            result = ptr.As<v8::External>()->Value();
        }

        return result;
    }

    void v8Getter(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::EscapableHandleScope hs(args.GetIsolate());
        DataTypeField* field = (DataTypeField*)args.Data().As<v8::External>()->Value();
        if (!field) {
            v8Throw(args.GetIsolate(), "Failed to get field information when getting a property of a host object");
            return;
        }

        DataType* type = nullptr;
        void* self = extractThisPointer(args.This(), args.GetIsolate(), &type);
        if (!type || !self) {
            v8Throw(args.GetIsolate(), "Failed to find internal fields when getting property '%s' of a host object", field->name.c_str());
            return;
        }

        v8::Local<v8::Value> result;
        utils::String failurePath;
        
        if (!convertToV8((u8*)self + field->offset, &result, field->type, field->flags.is_pointer, args.GetIsolate(), failurePath, nullptr)) {
            v8Throw(args.GetIsolate(), "Failed to convert '%s' from host type to script type", failurePath.c_str());
            return;
        }

        hs.Escape(result);
        args.GetReturnValue().Set(result);
    }

    void v8Setter(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::EscapableHandleScope hs(args.GetIsolate());
        DataTypeField* field = (DataTypeField*)args.Data().As<v8::External>()->Value();
        if (!field) {
            v8Throw(args.GetIsolate(), "Failed to get field information when setting a property of a host object");
            return;
        }

        DataType* type = nullptr;
        void* self = extractThisPointer(args.This(), args.GetIsolate(), &type);
        if (!type || !self) {
            v8Throw(args.GetIsolate(), "Failed to find internal fields when setting property '%s' of a host object", field->name.c_str());
            return;
        }

        v8::Local<v8::Value> result;
        utils::String failurePath;
        utils::String failureReason;
        utils::Array<ConvAlloc> allocs;
        if (!convertFromV8((void**)(u8*)self + field->offset, args[0], field->type, field->flags.is_pointer, allocs, args.GetIsolate(), failurePath, failureReason, nullptr)) {
            if (failureReason.size() > 0) {
                v8Throw(args.GetIsolate(), "Failed to convert '%s' from script type to host type with reason: %s", failurePath.c_str(), failureReason.c_str());
            } else {
                v8Throw(args.GetIsolate(), "Failed to convert '%s' from script type to host type", failurePath.c_str());
            }
            return;
        }

        args.GetReturnValue().Set(args[0]);
    }

    void ObjectRefresher(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::HandleScope hs(args.GetIsolate());
        v8::Local<v8::Object> selfV = args.This();
        DataType* type = nullptr;
        void* self = extractThisPointer(selfV, args.GetIsolate(), &type);
        if (!type || !self) {
            v8Throw(args.GetIsolate(), "Failed to find internal fields when refreshing host object");
            return;
        }

        utils::Array<DataTypeField>& fields = type->getFields();
        for (DataTypeField& f : fields) {
            if (f.flags.use_v8_accessors == 0) {
                v8::Local<v8::Value> result;
                utils::String failurePath;
                
                if (!convertToV8((u8*)self + f.offset, &result, f.type, f.flags.is_pointer, args.GetIsolate(), failurePath, nullptr)) {
                    v8Throw(args.GetIsolate(), "Failed to convert '%s' from host type to script type while refreshing host object", failurePath.c_str());
                    return;
                }

                v8SetProp(selfV, f.name, result);
                continue;
            }
        }
    }
};