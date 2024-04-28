#include <script/IScriptAPI.h>
#include <events/IEvent.h>
#include <events/Timeout.h>
#include <utils/Singleton.hpp>
#include <utils/Timer.h>
#include <utils/Thread.h>

#include <filesystem>
#include <windows.h>
#include <psapi.h>
#include <stdarg.h>

namespace t4ext {
    ICallbackData::~ICallbackData() {
    }

    DataTypeField::DataTypeField(const utils::String& _name, DataType* _type, u32 _offset, u32 _flags) {
        name = _name;
        type = _type;
        offset = _offset;
        memset(&flags, 0, sizeof(_Flags));
        setFlags(_flags);
    }

    void DataTypeField::setFlags(u32 f) {
        if (f & Flags::IsReadOnly) flags.is_readonly = 1;
        if (f & Flags::IsPointer) flags.is_pointer = 1;
        if (f & Flags::IsNullable) flags.is_nullable = 1;
        if (f & Flags::UseV8Accessors) flags.use_v8_accessors = 1;
    }

    FunctionArgument::FunctionArgument(const utils::String& _name, DataType* _type, u32 _flags) {
        name = _name;
        type = _type;
        setFlags(_flags);
    }

    void FunctionArgument::setFlags(u32 f) {
        memset(&flags, 0, sizeof(_Flags));
        if (f & Flags::IsPointer) flags.is_pointer = 1;
        if (f & Flags::IsNullable) flags.is_nullable = 1;
    }

    GlobalVariable::GlobalVariable(const utils::String& _name, DataType* _type, u32 _address, u32 _flags) {
        name = _name;
        type = _type;
        address = _address;
        setFlags(_flags);
    }

    void GlobalVariable::setFlags(u32 f) {
        if (f & Flags::IsPointer) flags.is_pointer = 1;
        if (f & Flags::IsNullable) flags.is_nullable = 1;
    }

    //
    // FunctionSignature
    //

    FunctionSignature::FunctionSignature(
        DataType* thisTp,
        DataType* retTp,
        const utils::Array<FunctionArgument>& args,
        u32 flags
    ) : m_args(args) {
        m_thisTp = thisTp;
        m_retTp = retTp;
        m_deallocateReturnCallback = nullptr;
        memset(&m_flags, 0, sizeof(_Flags));
        setFlags(flags);
    }

    FunctionSignature::~FunctionSignature() {}

    
    void FunctionSignature::setFlags(u32 f) {
        if (f & Flags::ReturnsPointer) m_flags.returns_pointer = 1;
        if (f & Flags::DeallocateReturnAfterCall) m_flags.deallocate_return_after_call = 1;
    }

    FunctionSignature::_Flags& FunctionSignature::getFlags() {
        return m_flags;
    }

    utils::Array<FunctionArgument>& FunctionSignature::getArgs() {
        return m_args;
    }

    DataType* FunctionSignature::getThisTp() {
        return m_thisTp;
    }

    DataType* FunctionSignature::getRetTp() {
        return m_retTp;
    }

    void FunctionSignature::setReturnValueDeallocationCallback(ReturnValueDeallocationCallback deallocationCallback) {
        m_deallocateReturnCallback = deallocationCallback;
    }

    FunctionSignature::ReturnValueDeallocationCallback FunctionSignature::getReturnValueDeallocationCallback() {
        return m_deallocateReturnCallback;
    }

    bool FunctionSignature::returnsPointer() {
        return m_flags.returns_pointer;
    }



    //
    // Function
    //
    
    Function::Function(
        const utils::String& name,
        void* address,
        DataType* retTp,
        const utils::Array<FunctionArgument>& args,
        u32 flags,
        DataType* methodOf
    ) : m_sig(methodOf, retTp, args, flags) {
        m_name = name;
        m_addr = address;
    }

    Function::~Function() {
    }

    const utils::String& Function::getName() {
        return m_name;
    }

    FunctionSignature& Function::getSignature() {
        return m_sig;
    }
    
    Function* Function::setArgNames(const utils::Array<utils::String>& names) {
        utils::Array<FunctionArgument>& args = m_sig.getArgs();
        for (u32 i = 0;i < names.size();i++) {
            if (i >= args.size()) return this;
            args[i].name = names[i];
        }

        return this;
    }

    Function* Function::setArgNullable(u32 idx, bool nullable) {
        m_sig.getArgs()[idx].flags.is_nullable = nullable ? 1 : 0;
        return this;
    }

    Function* Function::setReturnNullable(bool nullable) {
        m_sig.getFlags().return_nullable = nullable ? 1 : 0;
        return this;
    }
    
    Function* Function::setReturnValueDeallocationCallback(FunctionSignature::ReturnValueDeallocationCallback deallocationCallback) {
        m_sig.setReturnValueDeallocationCallback(deallocationCallback);
        return this;
    }
    
    Function* Function::setSignatureFlags(u32 flags) {
        m_sig.setFlags(flags);
        return this;
    }

    void* Function::getAddress() {
        return m_addr;
    }



    //
    // DataType
    //

    DataType::DataType(IScriptAPI* api, const utils::String& name, u32 size, Primitive primitiveType) : m_cbSignature(nullptr, nullptr, {}, 0) {
        m_api = api;
        m_name = name;
        m_size = size;
        m_primitiveType = primitiveType;
        m_elementType = nullptr;
        m_elemTpPointers = false;
        m_isFunction = false;
        m_isArray = false;

        m_ffiType.size = m_ffiType.alignment = 0;
        m_ffiType.type = FFI_TYPE_STRUCT;
        m_ffiElems.push(nullptr);

        switch (m_primitiveType) {
            case Primitive::pt_char: { m_ffiType = ffi_type_schar; break; }
            case Primitive::pt_u8: { m_ffiType = ffi_type_uint8; break; }
            case Primitive::pt_i8: { m_ffiType = ffi_type_sint8; break; }
            case Primitive::pt_i16: { m_ffiType = ffi_type_sint16; break; }
            case Primitive::pt_i32: { m_ffiType = ffi_type_sint32; break; }
            case Primitive::pt_i64: { m_ffiType = ffi_type_sint64; break; }
            case Primitive::pt_u16: { m_ffiType = ffi_type_uint16; break; }
            case Primitive::pt_u32: { m_ffiType = ffi_type_uint32; break; }
            case Primitive::pt_u64: { m_ffiType = ffi_type_uint64; break; }
            case Primitive::pt_f32: { m_ffiType = ffi_type_float; break; }
            case Primitive::pt_f64: { m_ffiType = ffi_type_double; break; }
            case Primitive::pt_pointer: { m_ffiType = ffi_type_pointer; break; }
            case Primitive::pt_enum: {
                switch (size) {
                    case 1: { m_ffiType = ffi_type_uint8; break; }
                    case 2: { m_ffiType = ffi_type_uint16; break; }
                    case 4: { m_ffiType = ffi_type_uint32; break; }
                    case 8: { m_ffiType = ffi_type_uint64; break; }
                }
            }
        }

        memset(&m_flags, 0, sizeof(_Flags));
        m_flags.needs_host_construction = 1; // assume this for safety
    }
    
    DataType::DataType(IScriptAPI* api, DataType* retTp, const utils::Array<FunctionArgument>& args, u32 flags) : m_cbSignature(nullptr, retTp, args, flags) {
        m_api = api;
        m_size = sizeof(void*);
        m_primitiveType = Primitive::pt_none;
        m_elementType = nullptr;
        m_elemTpPointers = false;
        m_isFunction = true;
        m_isArray = false;
        m_ffiType = ffi_type_pointer;

        memset(&m_flags, 0, sizeof(_Flags));
        m_flags.needs_host_construction = 1;
    }

    DataType::DataType(IScriptAPI* api, DataType* elemTp, bool isPointer) : m_cbSignature(nullptr, nullptr, {}, 0) {
        m_api = api;
        m_size = sizeof(utils::Array<void*>);
        m_primitiveType = Primitive::pt_none;
        m_elementType = elemTp;
        m_elemTpPointers = isPointer;
        m_isFunction = true;
        m_isArray = false;
        m_isFunction = false;
        m_isArray = true;

        m_ffiType.size = m_ffiType.alignment = 0;
        m_ffiType.type = FFI_TYPE_STRUCT;
        m_ffiElems.push(&ffi_type_uint32);
        m_ffiElems.push(&ffi_type_uint32);
        m_ffiElems.push(&ffi_type_pointer);
        m_ffiElems.push(nullptr);
        m_ffiType.elements = m_ffiElems.data();
    }

    DataType::~DataType() {
        for (Function* fn : m_methods) delete fn;
    }

    void DataType::setFlags(u32 f) {
        if (f & Flags::HostConstructionNotRequired) m_flags.needs_host_construction = 0;
        if (f & Flags::IsHidden) m_flags.is_hidden = 1;
    }

    void DataType::addField(const DataTypeField& field) {
        if (m_primitiveType != Primitive::pt_none) return;
        m_fields.push(field);
        m_fields.last().flags.use_v8_accessors = 1;

        m_ffiElems[m_ffiElems.size() - 1] = &m_fields.last().type->m_ffiType;
        m_ffiElems.push(nullptr);

        m_ffiType.size = m_ffiType.alignment = 0;
        m_ffiType.type = FFI_TYPE_STRUCT;
        m_ffiType.elements = m_ffiElems.data();
    }

    void DataType::addMethod(Function* method) {
        if (m_primitiveType != Primitive::pt_none) return;
        m_methods.push(method);
    }
    
    void DataType::addBase(DataType* base) {
        m_bases.push(base);
    }

    void DataType::bindEnumValue(const utils::String& name, u32 enumValue) {
        if (m_primitiveType != Primitive::pt_enum) return;
        m_fields.push(DataTypeField(
            name,
            m_api->getType<u32>(),
            enumValue,
            0
        ));
    }
    
    IScriptAPI* DataType::getApi() {
        return m_api;
    }

    ffi_type* DataType::getFFI() {
        return &m_ffiType;
    }

    const utils::String& DataType::getName() {
        return m_name;
    }

    u32 DataType::getSize() {
        return m_size;
    }

    bool DataType::isPrimitive() {
        return m_primitiveType != Primitive::pt_none;
    }

    bool DataType::isFunction() {
        return m_isFunction;
    }

    bool DataType::isArray() {
        return m_isArray;
    }

    Primitive DataType::getPrimitiveType() {
        return m_primitiveType;
    }
    
    FunctionSignature& DataType::getSignature() {
        return m_cbSignature;
    }
    
    DataType* DataType::getElementType() {
        return m_elementType;
    }
    
    bool DataType::areElementsPointers() {
        return m_elemTpPointers;
    }

    DataType::_Flags& DataType::getFlags() {
        return m_flags;
    }

    utils::Array<DataTypeField>& DataType::getFields() {
        return m_fields;
    }

    utils::Array<Function*>& DataType::getMethods() {
        return m_methods;
    }

    utils::Array<DataType*>& DataType::getBases() {
        return m_bases;
    }



    //
    // IScriptAPI
    //

    IScriptAPI::IScriptAPI() : utils::IWithLogging("ScriptAPI"), m_batchFlag(false), m_shouldTerminate(false) {
        // make sure the 0th namespace is the global one, and that it's current
        beginNamespace("");
    }

    IScriptAPI::~IScriptAPI() {
        for (ScriptNamespace* ns : m_namespaces) {
            for (DataType* tp : ns->types) delete tp;
            for (Function* fn : ns->globalFunctions) delete fn;
            delete ns;
        }
    }

    bool IScriptAPI::initPaths() {
        char procPath[MAX_PATH] = { 0 };
        if (GetModuleFileNameEx(GetCurrentProcess(), nullptr, procPath, MAX_PATH) == 0) {
            error("Failed to get process path");
            return false;
        }

        m_exePath = std::filesystem::u8path(procPath).string();
        m_pathBase = std::filesystem::u8path(procPath).remove_filename().string();
        
        for (u32 i = 0;i < m_exePath.size();i++) {
            if (m_exePath[i] == '\\') m_exePath[i] = '/';
        }
        for (u32 i = 0;i < m_pathBase.size();i++) {
            if (m_pathBase[i] == '\\') m_pathBase[i] = '/';
        }

        if (m_pathBase.size() > 0 && m_pathBase[m_pathBase.size() - 1] == '/') {
            m_pathBase = utils::String::View(m_pathBase.c_str(), m_pathBase.size() - 1);
        }
        m_scriptPath = m_pathBase + "/scripts";
        
        log("exe: %s", m_exePath.c_str());
        log("base: %s", m_pathBase.c_str());
        log("script: %s", m_scriptPath.c_str());

        return true;
    }

    const utils::String& IScriptAPI::getGameExecutablePath() {
        return m_exePath;
    }

    const utils::String& IScriptAPI::getGameBasePath() {
        return m_pathBase;
    }

    const utils::String& IScriptAPI::getScriptPath() {
        return m_scriptPath;
    }
    
    void IScriptAPI::beginNamespace(const utils::String& name) {
        for (ScriptNamespace* ns : m_namespaces) {
            if (ns->name == name) {
                m_currentNamespace = ns;
                return;
            }
        }

        m_namespaces.push(new ScriptNamespace({ name }));
        m_currentNamespace = m_namespaces.last();
    }

    void IScriptAPI::endNamespace() {
        m_currentNamespace = m_namespaces[0];
    }

    void IScriptAPI::addType(DataType* tp) {
        if (!m_currentNamespace) {
            error("No namespace has been started");
            return;
        }
        
        m_currentNamespace->types.push(tp);
    }

    void IScriptAPI::addFunc(Function* fn) {
        if (!m_currentNamespace) {
            error("No namespace has been started");
            return;
        }

        if (fn->getSignature().getFlags().returns_pointer) {
            // assume nullable for safety
            fn->setReturnNullable(true);
        }
        
        m_currentNamespace->globalFunctions.push(fn);
    }

    void IScriptAPI::dispatchEvent(IEvent* event) {
        m_events.push(event);
    }

    bool IScriptAPI::handleEvents() {
        utils::Singleton<TimeoutEventType>::Get()->processTimeouts();
        
        std::unique_lock<std::mutex> lock(m_batchMutex);
        m_batchCondition.wait(lock, [this](){ return m_batchFlag == true || m_shouldTerminate == true; });
        if (m_batchFlag == false || m_shouldTerminate) {
            return true;
        }

        utils::Timer processTimer;
        processTimer.start();

        while (true) {
            IEvent* nextEvent = nullptr;
            if (m_events.size() > 0) nextEvent = m_events.pop();

            if (!nextEvent) {
                // No more events to process
                break;
            }

            nextEvent->process(this);

            if (processTimer.elapsed() > 0.01666666666f) {
                // Don't take too long... Save the rest for next time
                break;
            }
        }

        m_batchFlag = !m_batchFlag;
        m_batchCondition.notify_one();

        return m_shouldTerminate == true;
    }
    
    bool IScriptAPI::initialize() {
        return true;
    }
    
    bool IScriptAPI::commitBindings() {
        return true;
    }
    
    bool IScriptAPI::shutdown() {
        return true;
    }
    
    bool IScriptAPI::executeEntry() {
        return true;
    }
    
    void IScriptAPI::scriptException(const utils::String& msg) {
    }
    
    void IScriptAPI::scriptExceptionf(const char* fmt, ...) {
        char buf[512] = { 0 };

        va_list ap;
        va_start(ap, fmt);
        vsnprintf(buf, 512, fmt, ap);
        scriptException(utils::String(fmt));
        va_end(ap);
    }

    void IScriptAPI::signalTermination() {
        m_shouldTerminate = true;
        m_batchCondition.notify_one();
    }
    
    void IScriptAPI::signalEventBatchStart() {
        if (m_shouldTerminate) return;
        m_batchFlag = !m_batchFlag;
        m_batchCondition.notify_one();
    }
    
    void IScriptAPI::waitForEventBatchCompletion() {
        if (m_shouldTerminate) return;
        std::unique_lock<std::mutex> lock(m_batchMutex);
        m_batchCondition.wait_for(lock, std::chrono::milliseconds(16), [this](){ return m_batchFlag == false; });
    }
};