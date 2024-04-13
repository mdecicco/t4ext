#include <script/IScriptAPI.h>
#include <events/IEvent.h>
#include <utils/Timer.h>

#include <filesystem>
#include <windows.h>
#include <psapi.h>

namespace t4ext {
    //
    // FunctionSignature
    //

    FunctionSignature::FunctionSignature(DataType* thisTp, DataType* retTp, bool returnsPtr, const utils::Array<FunctionArgument>& args)
        : m_thisTp(thisTp), m_retTp(retTp), m_returnsPtr(returnsPtr), m_args(args) {}
    FunctionSignature::~FunctionSignature() {}

    utils::Array<FunctionArgument>& FunctionSignature::getArgs() {
        return m_args;
    }

    DataType* FunctionSignature::getThisTp() {
        return m_thisTp;
    }

    DataType* FunctionSignature::getRetTp() {
        return m_retTp;
    }

    bool FunctionSignature::returnsPointer() {
        return m_returnsPtr;
    }



    //
    // Function
    //
    
    Function::Function(const utils::String& name, void* address, DataType* retTp, bool returnsPtr, const utils::Array<FunctionArgument>& args, DataType* methodOf) : m_sig(methodOf, retTp, returnsPtr, args) {
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
    
    void Function::setArgNames(const utils::Array<utils::String>& names) {
        utils::Array<FunctionArgument>& args = m_sig.getArgs();
        for (u32 i = 0;i < names.size();i++) {
            if (i >= args.size()) return;
            args[i].name = names[i];
        }
    }

    void* Function::getAddress() {
        return m_addr;
    }



    //
    // DataType
    //

    DataType::DataType(IScriptAPI* api, const utils::String& name, u32 size, Primitive primitiveType) {
        m_api = api;
        m_name = name;
        m_size = size;
        m_primitiveType = primitiveType;
    }

    DataType::~DataType() {
        for (Function* fn : m_methods) delete fn;
    }

    void DataType::addField(const DataTypeField& field) {
        if (m_primitiveType != Primitive::pt_none) return;
        m_fields.push(field);
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
        m_fields.push({
            name,
            m_api->getType<u32>(),
            enumValue,
            { 0, 0 }
        });
    }
    
    IScriptAPI* DataType::getApi() {
        return m_api;
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

    Primitive DataType::getPrimitiveType() {
        return m_primitiveType;
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

    IScriptAPI::IScriptAPI() : utils::IWithLogging("ScriptAPI") {
    }

    IScriptAPI::~IScriptAPI() {
        for (DataType* tp : m_types) delete tp;
        for (Function* fn : m_globalFunctions) delete fn;
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
        m_entryPath = m_pathBase + "/scripts/main.ts";
        
        log("base: %s", m_pathBase.c_str());
        log("exe: %s", m_exePath.c_str());
        log("entry: %s", m_entryPath.c_str());

        return true;
    }

    const utils::String& IScriptAPI::getGameExecutablePath() {
        return m_exePath;
    }

    const utils::String& IScriptAPI::getGameBasePath() {
        return m_pathBase;
    }

    const utils::String& IScriptAPI::getScriptEntrypointPath() {
        return m_entryPath;
    }

    void IScriptAPI::addType(DataType* tp) {
        m_types.push(tp);
    }

    void IScriptAPI::addFunc(Function* fn) {
        m_globalFunctions.push(fn);
    }

    void IScriptAPI::dispatchEvent(IEvent* event) {
        m_eventMutex.lock();
        m_events.push(event);
        m_eventMutex.unlock();
    }

    void IScriptAPI::handleEvents() {
        utils::Timer processTimer;
        processTimer.start();

        while (true) {
            IEvent* nextEvent = nullptr;
            m_eventMutex.lock();
            if (m_events.size() > 0) nextEvent = m_events.pop();
            m_eventMutex.unlock();

            if (!nextEvent) {
                // No more events to process
                break;
            }

            nextEvent->process(this);
            delete nextEvent;

            if (processTimer.elapsed() > 0.01666666666f) {
                // Don't take too long... Save the rest for next time
                break;
            }
        }
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
};