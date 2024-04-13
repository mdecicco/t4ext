#pragma once
#include <types.h>

#include <utils/Array.hpp>
#include <utils/String.h>
#include <utils/ILogListener.h>
#include <utils/robin_hood.h>

#include <type_traits>

namespace t4ext {
    class IEventType;
    class IEvent;
    class DataType;
    class IScriptAPI;

    enum class Primitive {
        pt_none,
	    pt_char,
	    pt_i8,
	    pt_i16,
	    pt_i32,
	    pt_i64,
	    pt_u8,
	    pt_u16,
	    pt_u32,
	    pt_u64,
        pt_enum,
	    pt_f32,
	    pt_f64,
	    pt_pointer
    };

    struct DataTypeField {
        utils::String name;
        DataType* type;
        u32 offset;
        struct {
            unsigned is_readonly : 1;
            unsigned is_pointer  : 1;
            unsigned is_nullable : 1;
        } flags;
    };

    struct FunctionArgument {
        DataType* type;
        utils::String name;
        struct {
            unsigned is_pointer  : 1;
            unsigned is_nullable : 1;
        } flags;
    };

    class FunctionSignature {
        public:
            FunctionSignature(DataType* thisTp, DataType* retTp, bool returnsPtr, const utils::Array<FunctionArgument>& args);
            ~FunctionSignature();

            utils::Array<FunctionArgument>& getArgs();
            DataType* getThisTp();
            DataType* getRetTp();
            bool returnsPointer();

        protected:
            DataType* m_thisTp;
            DataType* m_retTp;
            bool m_returnsPtr;
            utils::Array<FunctionArgument> m_args;
    };

    class Function {
        public:
            Function(const utils::String& name, void* address, DataType* retTp, bool returnsPtr, const utils::Array<FunctionArgument>& args, DataType* methodOf = nullptr);
            ~Function();

            const utils::String& getName();
            FunctionSignature& getSignature();
            void setArgNames(const utils::Array<utils::String>& names);
            void* getAddress();

        protected:
            utils::String m_name;
            void* m_addr;
            FunctionSignature m_sig;
    };

    class DataType {
        public:
            DataType(IScriptAPI* api, const utils::String& name, u32 size, Primitive primitiveType = Primitive::pt_none);
            ~DataType();

            void addField(const DataTypeField& field);
            void addMethod(Function* method);
            void addBase(DataType* base);
            void bindEnumValue(const utils::String& name, u32 enumValue);

            IScriptAPI* getApi();
            const utils::String& getName();
            u32 getSize();
            bool isPrimitive();
            Primitive getPrimitiveType();
            utils::Array<DataTypeField>& getFields();
            utils::Array<Function*>& getMethods();
            utils::Array<DataType*>& getBases();

            template <typename Cls, typename Ret, typename ...Args>
            Function* bind(const utils::String& name, Ret(Cls::*fn)(Args...));

            template <typename Cls, typename T>
            DataTypeField* bind(const utils::String& name, T Cls::* property);

        protected:
            IScriptAPI* m_api;
            utils::String m_name;
            u32 m_size;
            Primitive m_primitiveType;
            utils::Array<DataTypeField> m_fields;
            utils::Array<Function*> m_methods;
            utils::Array<DataType*> m_bases;
    };

    struct GlobalVariable {
        utils::String name;
        DataType* type;
        u32 address;
        struct {
            unsigned is_pointer  : 1;
            unsigned is_nullable : 1;
        } flags;
    };

    class IScriptAPI : public utils::IWithLogging {
        public:
            IScriptAPI();
            virtual ~IScriptAPI();

            bool initPaths();
            const utils::String& getGameExecutablePath();
            const utils::String& getGameBasePath();
            const utils::String& getScriptEntrypointPath();

            void addType(DataType* tp);
            void addFunc(Function* fn);

            template <typename T>
            DataType* getType();

            template <typename T>
            std::enable_if_t<std::is_class_v<T>, DataType*>
            bind(const utils::String& name);

            template <typename T>
            GlobalVariable* bind(const utils::String& name, T* global);

            template <typename Ret, typename ...Args>
            Function* bind(const utils::String& name, Ret(*fn)(Args...));

            template <typename T>
            std::enable_if_t<std::is_fundamental_v<T> || std::is_same_v<T, pointer>, DataType*>
            bind(const utils::String& name);
            
            template <typename T>
            std::enable_if_t<std::is_enum_v<T>, DataType*>
            bind(const utils::String& name);

            template <typename T>
            std::enable_if_t<std::is_base_of_v<IEvent, T> || std::is_same_v<T, IEvent>, void>
            registerEventType(IEventType* eventType, const utils::String& name);

            virtual void dispatchEvent(IEvent* event);
            virtual void handleEvents();
            virtual bool initialize();
            virtual bool commitBindings();
            virtual bool shutdown();
            virtual bool executeEntry();

        protected:
            utils::Array<DataType*> m_types;
            utils::Array<Function*> m_globalFunctions;
            utils::Array<GlobalVariable> m_globalVars;
            robin_hood::unordered_flat_map<size_t, DataType*> m_typeMap;

            std::mutex m_eventMutex;
            utils::Array<IEvent*> m_events;
            utils::Array<IEventType*> m_eventTypes;

            utils::String m_exePath;
            utils::String m_pathBase;
            utils::String m_entryPath;
    };
};