#pragma once
#include <script/IScriptAPI.h>
#include <events/IEvent.h>

namespace t4ext {
    template <typename Cls, typename Ret, typename ...Args>
    Function* DataType::bind(const utils::String& name, Ret(Cls::*fn)(Args...)) {
        DataType* retTp = m_api->getType<Ret>();
        bool returnsPtr = std::is_pointer_v<Ret> || std::is_reference_v<Ret>;
        utils::Array<DataType*> argTps = { m_api->getType<Args>()... };
        utils::Array<bool> argIsPtr = { (std::is_pointer_v<Args> || std::is_reference_v<Args>)... };
        const char* argTpInfo[std::tuple_size_v<std::tuple<Args...>> + 1] = { typeid(std::remove_cvref_t<std::remove_pointer_t<Args>>).name()..., nullptr };

        if (!std::is_same_v<Ret, void> && !retTp) {
            m_api->error("Return type of function %s is '%s', which has not been bound", name.c_str(), typeid(std::remove_cvref_t<std::remove_pointer_t<Ret>>).name());
        }

        for (u32 i = 0;i < argTps.size();i++) {
            if (argTps[i] == nullptr) {
                m_api->error("Argument #%d of function %s is of type '%s', which has not been bound", i + 1, name.c_str(), argTpInfo[i]);
                return nullptr;
            }
        }

        Function* bfn = new Function(
            name,
            (void*&)fn,
            retTp,
            returnsPtr,
            argTps.map([argIsPtr](DataType* tp, u32 idx) {
                return FunctionArgument({
                    tp,
                    utils::String::Format("param_%d", idx + 1),
                    {
                        argIsPtr[idx],
                        0 // assume non-nullable for safety
                    }
                });
            }),
            this
        );

        addMethod(bfn);

        return bfn;
    }

    
    template <typename Cls, typename T>
    DataTypeField* DataType::bind(const utils::String& name, T Cls::* property) {
        DataType* tp = m_api->getType<T>();
        if (!tp) {
            m_api->error(
                "Property '%s' of type '%s' is of type '%s', which has not been bound",
                name.c_str(), m_name.c_str(), typeid(std::remove_cvref_t<std::remove_pointer_t<T>>).name()
            );
            return nullptr;
        }

        u32 offset = (u8*)&((Cls*)nullptr->*property) - (u8*)nullptr;
        addField({
            name,
            tp,
            offset,
            {
                0,
                (std::is_pointer_v<T> || std::is_reference_v<T>) ? 1 : 0,
                1 // assume nullable for safety
            }
        });

        return &m_fields.last();
    }


    template <typename T>
    DataType* IScriptAPI::getType() {
        size_t hash = typeid(std::remove_cvref_t<std::remove_pointer_t<T>>).hash_code();
        auto it = m_typeMap.find(hash);
        if (it == m_typeMap.end()) return nullptr;

        return it->second;
    }

    template <typename T>
    std::enable_if_t<std::is_class_v<T>, DataType*>
    IScriptAPI::bind(const utils::String& name) {
        size_t hash = typeid(std::remove_cvref_t<std::remove_pointer_t<T>>).hash_code();
        if (m_typeMap.count(hash) > 0) return nullptr;

        DataType* tp = new DataType(this, name, sizeof(std::remove_cvref_t<std::remove_pointer_t<T>>));
        addType(tp);
        m_typeMap.insert(robin_hood::pair<size_t, DataType*>(hash, tp));

        return tp;
    }

    template <typename T>
    GlobalVariable* IScriptAPI::bind(const utils::String& name, T* global) {
        DataType* tp = getType<T>();
        if (!tp) {
            error("Global variable %s is of type '%s', which has not been bound", name.c_str(), typeid(std::remove_cvref_t<std::remove_pointer_t<T>>).name());
            return nullptr;
        }

        m_globalVars.push({
            name,
            tp,
            reinterpret_cast<u32>(global),
            {
                (std::is_pointer_v<T> || std::is_reference_v<T>) ? 1 : 0,
                1 // assume nullable for safety
            }
        });

        return &m_globalVars.last();
    }

    template <typename Ret, typename ...Args>
    Function* IScriptAPI::bind(const utils::String& name, Ret(*fn)(Args...)) {
        DataType* retTp = getType<Ret>();
        bool returnsPtr = std::is_pointer_v<Ret> || std::is_reference_v<Ret>;
        utils::Array<DataType*> argTps = { getType<Args>()... };
        utils::Array<bool> argIsPtr = { (std::is_pointer_v<Args> || std::is_reference_v<Args>)... };
        const char* argTpInfo[std::tuple_size_v<std::tuple<Args...>> + 1] = { typeid(std::remove_cvref_t<std::remove_pointer_t<Args>>).name()..., nullptr };

        if (!std::is_same_v<Ret, void> && !retTp) {
            error("Return type of function %s is '%s', which has not been bound", name.c_str(), typeid(std::remove_cvref_t<std::remove_pointer_t<Ret>>).name());
        }

        for (u32 i = 0;i < argTps.size();i++) {
            if (argTps[i] == nullptr) {
                error("Argument #%d of function %s is of type '%s', which has not been bound", i + 1, name.c_str(), argTpInfo[i]);
                return nullptr;
            }
        }

        Function* bfn = new Function(
            name,
            (void*)fn,
            retTp,
            returnsPtr,
            argTps.map([argIsPtr](DataType* tp, u32 idx) {
                return FunctionArgument({
                    tp,
                    utils::String::Format("param_%d", idx + 1),
                    {
                        argIsPtr[idx],
                        0 // assume non-nullable for safety
                    }
                });
            })
        );

        addFunc(bfn);

        return bfn;
    }

    template <typename T>
    std::enable_if_t<std::is_fundamental_v<T> || std::is_same_v<T, pointer>, DataType*>
    IScriptAPI::bind(const utils::String& name) {
        size_t hash = typeid(T).hash_code();
        if (m_typeMap.count(hash) > 0) return nullptr;

        Primitive p = Primitive::pt_none;
        if constexpr (std::is_same_v<T, char>) p = Primitive::pt_char;
        else if constexpr (std::is_same_v<T, i8>) p = Primitive::pt_i8;
        else if constexpr (std::is_same_v<T, i16>) p = Primitive::pt_i16;
        else if constexpr (std::is_same_v<T, i32>) p = Primitive::pt_i32;
        else if constexpr (std::is_same_v<T, i64>) p = Primitive::pt_i64;
        else if constexpr (std::is_same_v<T, u8>) p = Primitive::pt_u8;
        else if constexpr (std::is_same_v<T, u16>) p = Primitive::pt_u16;
        else if constexpr (std::is_same_v<T, u32>) p = Primitive::pt_u32;
        else if constexpr (std::is_same_v<T, u64>) p = Primitive::pt_u64;
        else if constexpr (std::is_same_v<T, f32>) p = Primitive::pt_f32;
        else if constexpr (std::is_same_v<T, f64>) p = Primitive::pt_f64;
        else if constexpr (std::is_same_v<T, pointer>) p = Primitive::pt_pointer;
        else {
            error("Primitive type %s is of type '%s', which is not supported", name.c_str(), typeid(T).name());
            return nullptr;
        }

        DataType* tp = new DataType(this, name, sizeof(T), p);
        addType(tp);
        m_typeMap.insert(robin_hood::pair<size_t, DataType*>(hash, tp));

        return tp;
    }

    template <typename T>
    std::enable_if_t<std::is_enum_v<T>, DataType*>
    IScriptAPI::bind(const utils::String& name) {
        size_t hash = typeid(T).hash_code();
        if (m_typeMap.count(hash) > 0) return nullptr;

        DataType* tp = new DataType(this, name, sizeof(T), Primitive::pt_enum);
        addType(tp);
        m_typeMap.insert(robin_hood::pair<size_t, DataType*>(hash, tp));

        return tp;
    }

    template <typename T>
    std::enable_if_t<std::is_base_of_v<IEvent, T> || std::is_same_v<T, IEvent>, void>
    IScriptAPI::registerEventType(IEventType* eventType, const utils::String& name) {
        eventType->setName(name);
        
        DataType* tp = bind<T>(name);
        eventType->bind(tp);
    }
}