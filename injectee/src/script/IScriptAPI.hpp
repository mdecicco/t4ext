#pragma once
#include <script/IScriptAPI.h>
#include <script/CallScript.hpp>
#include <events/IEvent.h>

namespace t4ext {
    template <class T> struct callback_type {};
    template <typename Ret, typename... Args>
    struct callback_type<ICallback<Ret, Args...>> { using value_type = Ret(Args...); };

    template<class T> struct remove_all { typedef T type; };
    template<class T> struct remove_all<T*> : remove_all<T> {};
    template<class T> struct remove_all<T&> : remove_all<T> {};
    template<class T> struct remove_all<T&&> : remove_all<T> {};
    template<class T> struct remove_all<T const> : remove_all<T> {};
    template<class T> struct remove_all<T volatile> : remove_all<T> {};
    template<class T> struct remove_all<T const volatile> : remove_all<T> {};
    template<class T> struct remove_all<T[]> : remove_all<T> {};

    template<typename Fn> struct function_traits;

    // specialization for functions
    template<typename Ret, typename ...Args>
    struct function_traits<Ret (Args...)> {
        using result = Ret;

        static bool returnsPointer() {
            return std::is_pointer_v<Ret> || std::is_reference_v<Ret>;
        }

        static DataType* getRetType(IScriptAPI* api, bool& didError) {
            DataType* tp = api->getType<Ret>();
            if (!tp && !std::is_same_v<Ret, void>) {
                api->error(
                    "Return type of function signature is '%s', which has not been bound",
                    typeid(typename remove_all<Ret>::type).name()
                );

                didError = true;
            }

            return tp;
        }

        static utils::Array<FunctionArgument> getArgs(IScriptAPI* api, bool& didError) {
            utils::Array<FunctionArgument> args = {};

            constexpr int argc = std::tuple_size_v<std::tuple<Args...>>;
            if constexpr (argc == 0) return args;
            args.reserve(argc);

            if constexpr (argc > 0) {
                DataType* argTypes[] = { api->getType<typename remove_all<Args>::type>()... };
                const char* argTpNames[] = { typeid(typename remove_all<Args>::type).name()... };
                constexpr bool argIsPtr[] = { (std::is_reference_v<Args> || std::is_pointer_v<Args>)... };

                for (u8 a = 0;a < argc;a++) {
                    if (!argTypes[a]) {
                        api->error("Argument #%d of function signature is of type '%s', which has not been bound", a + 1, argTpNames[a]);
                        didError = true;
                    }

                    args.push(FunctionArgument(
                        utils::String::Format("param_%d", a),
                        argTypes[a],
                        argIsPtr ? FunctionArgument::Flags::IsPointer : FunctionArgument::Flags::None
                    ));
                }
            }

            return args;
        }
    };

    template <typename Ret, typename ...Args>
    ICallback<Ret, Args...>::~ICallback() {}

    template <typename Ret, typename ...Args>
    Ret ICallback<Ret, Args...>::operator()(Args... args) {
        // for now...
        return CallCallback<Ret, Args...>((TypeScriptCallback*)this, args...);
    }

    template <typename Ret, typename ...Args>
    Ret ICallback<Ret, Args...>::call(Args... args) {
        // for now...
        return CallCallback<Ret, Args...>((TypeScriptCallback*)this, args...);
    }

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

        u32 flags = 0;
        if (returnsPtr) flags |= FunctionSignature::Flags::ReturnsPointer;

        Function* bfn = new Function(
            name,
            (void*&)fn,
            retTp,
            argTps.map([argIsPtr](DataType* tp, u32 idx) {
                u32 flags = 0;
                if (argIsPtr[idx]) flags |= FunctionArgument::Flags::IsPointer;
                return FunctionArgument(
                    utils::String::Format("param_%d", idx + 1),
                    tp,
                    flags
                );
            }),
            flags,
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

        u32 flags = DataTypeField::Flags::IsNullable; // Assume nullable for safety
        if constexpr (std::is_pointer_v<T> || std::is_reference_v<T>) flags |= DataTypeField::Flags::IsPointer;

        addField(DataTypeField(name, tp, offset, flags));

        return &m_fields.last();
    }


    template <typename T>
    DataType* IScriptAPI::getType() {
        size_t hash = typeid(remove_all<T>::type).hash_code();

        if constexpr (is_callback_v<typename remove_all<T>::type>) {
            // automatically bind signature types for convenience because we can

            using traits = function_traits<typename callback_type<typename remove_all<T>::type>::value_type>;
            auto it = m_typeMap.find(hash);
            if (it != m_typeMap.end()) return it->second;

            bool didError = false;
            DataType* retTp = traits::getRetType(this, didError);
            if (didError) return nullptr;

            utils::Array<FunctionArgument> args = traits::getArgs(this, didError);
            if (didError) return nullptr;

            DataType* tp = new DataType(
                this,
                retTp,
                args,
                traits::returnsPointer() ? FunctionSignature::Flags::ReturnsPointer : FunctionSignature::Flags::None
            );

            addType(tp);

            m_typeMap.insert(robin_hood::pair<size_t, DataType*>(hash, tp));

            return tp;
        }

        auto it = m_typeMap.find(hash);
        if (it == m_typeMap.end()) return nullptr;

        return it->second;
    }

    template <typename T>
    std::enable_if_t<std::is_class_v<T>, DataType*>
    IScriptAPI::bind(const utils::String& name) {
        if (!m_currentNamespace) {
            error("No namespace has been started");
            return nullptr;
        }

        size_t hash = typeid(remove_all<T>::type).hash_code();
        if (m_typeMap.count(hash) > 0) return nullptr;

        DataType* tp = new DataType(this, name, sizeof(remove_all<T>::type));
        addType(tp);
        m_typeMap.insert(robin_hood::pair<size_t, DataType*>(hash, tp));

        return tp;
    }

    template <typename T>
    GlobalVariable* IScriptAPI::bind(const utils::String& name, T* global) {
        if (!m_currentNamespace) {
            error("No namespace has been started");
            return nullptr;
        }

        DataType* tp = getType<T>();
        if (!tp) {
            error("Global variable %s is of type '%s', which has not been bound", name.c_str(), typeid(remove_all<T>::type).name());
            return nullptr;
        }

        // assume nullable for safety
        m_currentNamespace->globalVars.push(GlobalVariable(name, tp, reinterpret_cast<u32>(global), GlobalVariable::Flags::IsNullable));

        return &m_currentNamespace->globalVars.last();
    }

    template <typename Ret, typename ...Args>
    Function* IScriptAPI::bind(const utils::String& name, Ret(*fn)(Args...)) {
        if (!m_currentNamespace) {
            error("No namespace has been started");
            return nullptr;
        }
        
        DataType* retTp = getType<Ret>();
        bool returnsPtr = std::is_pointer_v<Ret> || std::is_reference_v<Ret>;
        utils::Array<DataType*> argTps = { getType<Args>()... };
        utils::Array<bool> argIsPtr = { (std::is_pointer_v<Args> || std::is_reference_v<Args>)... };
        const char* argTpInfo[std::tuple_size_v<std::tuple<Args...>> + 1] = { typeid(remove_all<Args>::type).name()..., nullptr };

        if (!std::is_same_v<Ret, void> && !retTp) {
            error("Return type of function %s is '%s', which has not been bound", name.c_str(), typeid(remove_all<Ret>::type).name());
        }

        for (u32 i = 0;i < argTps.size();i++) {
            if (argTps[i] == nullptr) {
                error("Argument #%d of function %s is of type '%s', which has not been bound", i + 1, name.c_str(), argTpInfo[i]);
                return nullptr;
            }
        }

        u32 flags = 0;
        if (returnsPtr) flags |= FunctionSignature::Flags::ReturnsPointer;

        Function* bfn = new Function(
            name,
            (void*)fn,
            retTp,
            argTps.map([argIsPtr](DataType* tp, u32 idx) {
                u32 flags = 0;
                if (argIsPtr[idx]) flags |= FunctionArgument::Flags::IsPointer;
                return FunctionArgument(
                    utils::String::Format("param_%d", idx + 1),
                    tp,
                    flags
                );
            }),
            flags
        );

        addFunc(bfn);

        return bfn;
    }

    template <typename T>
    std::enable_if_t<std::is_fundamental_v<T> || std::is_same_v<T, pointer> || std::is_same_v<T, void*>, DataType*>
    IScriptAPI::bind(const utils::String& name) {
        if (!m_currentNamespace) {
            error("No namespace has been started");
            return nullptr;
        }
        
        size_t hash = typeid(remove_all<T>::type).hash_code();
        if (m_typeMap.count(hash) > 0) return nullptr;

        Primitive p = Primitive::pt_none;
        if constexpr (std::is_same_v<typename remove_all<T>::type, char>) p = Primitive::pt_char;
        else if constexpr (std::is_same_v<typename remove_all<T>::type, bool>) p = Primitive::pt_u8;
        else if constexpr (std::is_same_v<typename remove_all<T>::type, i8>) p = Primitive::pt_i8;
        else if constexpr (std::is_same_v<typename remove_all<T>::type, i16>) p = Primitive::pt_i16;
        else if constexpr (std::is_same_v<typename remove_all<T>::type, i32>) p = Primitive::pt_i32;
        else if constexpr (std::is_same_v<typename remove_all<T>::type, i64>) p = Primitive::pt_i64;
        else if constexpr (std::is_same_v<typename remove_all<T>::type, u8>) p = Primitive::pt_u8;
        else if constexpr (std::is_same_v<typename remove_all<T>::type, u16>) p = Primitive::pt_u16;
        else if constexpr (std::is_same_v<typename remove_all<T>::type, u32>) p = Primitive::pt_u32;
        else if constexpr (std::is_same_v<typename remove_all<T>::type, u64>) p = Primitive::pt_u64;
        else if constexpr (std::is_same_v<typename remove_all<T>::type, f32>) p = Primitive::pt_f32;
        else if constexpr (std::is_same_v<typename remove_all<T>::type, f64>) p = Primitive::pt_f64;
        else if constexpr (std::is_same_v<typename remove_all<T>::type, pointer>) p = Primitive::pt_pointer;
        else {
            error("Primitive type %s is of type '%s', which is not supported", name.c_str(), typeid(remove_all<T>::type).name());
            return nullptr;
        }

        DataType* tp = new DataType(this, name, sizeof(remove_all<T>::type), p);
        addType(tp);
        m_typeMap.insert(robin_hood::pair<size_t, DataType*>(hash, tp));

        return tp;
    }

    template <typename T>
    std::enable_if_t<std::is_enum_v<T>, DataType*>
    IScriptAPI::bind(const utils::String& name) {
        if (!m_currentNamespace) {
            error("No namespace has been started");
            return nullptr;
        }
        
        size_t hash = typeid(remove_all<T>::type).hash_code();
        if (m_typeMap.count(hash) > 0) return nullptr;

        DataType* tp = new DataType(this, name, sizeof(remove_all<T>::type), Primitive::pt_enum);
        addType(tp);
        m_typeMap.insert(robin_hood::pair<size_t, DataType*>(hash, tp));

        return tp;
    }

    template <typename T>
    std::enable_if_t<std::is_base_of_v<IEvent, T> || std::is_same_v<T, IEvent>, void>
    IScriptAPI::registerEventType(IEventType* eventType, const utils::String& name) {
        eventType->setName(name);
        
        beginNamespace("t4");
        DataType* tp = bind<T>(name);
        eventType->bind(tp);
        endNamespace();
    }
}