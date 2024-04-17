#pragma once
#include <types.h>

#include <utils/Array.h>
#include <utils/String.h>

#include <v8.h>

namespace t4ext {
    class DataType;
    class DataTypeField;

    struct ConvAlloc {
        u8* alloc; // free with delete [] if doFree
        bool doFree;
    };
    
    bool convertToV8(
        void* hostObj,
        v8::Local<v8::Value>* out,
        DataType* tp,
        bool isPtr,
        v8::Isolate* isolate,
        utils::String& failurePath,
        DataTypeField* selfField = nullptr
    );

    /**
     * If the destination is a primitive value, and _not_ a pointer to a primitive,
     * then `*destObj` will be set to the VALUE of the primitive and not a pointer
     * to memory which holds the value. You will have to cast the `void*` itself to
     * the primitive rather than dereferencing it to get the value.
     * 
     * Related to the above: Because 32-bits, `void*` isn't a wide enough type to
     * store 64-bit primitives, so this function will give an error result if any
     * non-pointer 64-bit primitives are encountered.
     * 
     * Don't preallocate the memory to be used to store the constructed type, this
     * function will handle that if non-primitive.
     */
    bool convertFromV8(
        void** destObj,
        const v8::Local<v8::Value>& in,
        DataType* tp,
        bool expectsPtr,
        utils::Array<ConvAlloc>& outAllocs,
        v8::Isolate* isolate,
        utils::String& failurePath,
        utils::String& failureReason,
        DataTypeField* selfField = nullptr
    );
    void specifyThisPointer(v8::Local<v8::Object>& onObject, void* thisPointer, DataType* type, v8::Isolate* isolate);
    void* extractThisPointer(const v8::Local<v8::Object>& maybeThisObj, v8::Isolate* isolate, DataType** outDataType = nullptr);
};