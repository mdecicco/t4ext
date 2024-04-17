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

    template <typename Ret, typename ...Args>
    class ICallback {
        public:
            virtual ~ICallback();

            // proxy for call method
            Ret operator()(Args...);
            Ret call(Args...);
    };

    template <typename C> struct is_callback : std::false_type {};
    template <typename Ret, typename... Args> struct is_callback<ICallback<Ret, Args...>> : std::true_type {};
    template <typename C> inline constexpr bool is_callback_v = is_callback<C>::value;

    struct DataTypeField {
        struct _Flags {
            unsigned is_readonly : 1;
            unsigned is_pointer  : 1;
            unsigned is_nullable : 1;
            unsigned _unused : 5;
        };
        enum Flags {
            None = 0,
            IsReadOnly = 0b00000001,
            IsPointer  = 0b00000010,
            IsNullable = 0b00000100
        };

        DataTypeField(const utils::String& name, DataType* type, u32 offset, u32 flags);
        void setFlags(u32 f);

        utils::String name;
        DataType* type;
        u32 offset;
        _Flags flags;
    };

    struct FunctionArgument {
        struct _Flags {
            unsigned is_pointer  : 1;
            unsigned is_nullable : 1;
            unsigned _unused : 5;
        };
        enum Flags {
            None = 0,
            IsPointer  = 0b00000001,
            IsNullable = 0b00000010
        };

        FunctionArgument(const utils::String& name, DataType* type, u32 flags);
        void setFlags(u32 f);
        
        utils::String name;
        DataType* type;
        _Flags flags;
    };
    
    struct GlobalVariable {
        struct _Flags {
            unsigned is_pointer  : 1;
            unsigned is_nullable : 1;
            unsigned _unused : 5;
        };
        enum Flags {
            None = 0,
            IsPointer  = 0b00000001,
            IsNullable = 0b00000010
        };

        GlobalVariable(const utils::String& name, DataType* type, u32 address, u32 flags);
        void setFlags(u32 f);

        utils::String name;
        DataType* type;
        u32 address;
        _Flags flags;
    };

    class FunctionSignature {
        public:
            struct _Flags {
                unsigned returns_pointer : 1;
                unsigned _unused : 7;
            };
            enum Flags {
                None = 0,
                ReturnsPointer = 0b00000001,
            };

            FunctionSignature(DataType* thisTp, DataType* retTp, const utils::Array<FunctionArgument>& args, u32 flags);
            ~FunctionSignature();

            void setFlags(u32 f);
            _Flags& getFlags();
            utils::Array<FunctionArgument>& getArgs();
            DataType* getThisTp();
            DataType* getRetTp();
            bool returnsPointer();

        protected:
            DataType* m_thisTp;
            DataType* m_retTp;
            _Flags m_flags;
            utils::Array<FunctionArgument> m_args;
    };

    class Function {
        public:
            Function(const utils::String& name, void* address, DataType* retTp, const utils::Array<FunctionArgument>& args, u32 flags, DataType* methodOf = nullptr);
            ~Function();

            const utils::String& getName();
            FunctionSignature& getSignature();
            Function* setArgNames(const utils::Array<utils::String>& names);
            Function* setArgNullable(u32 idx);
            void* getAddress();

        protected:
            utils::String m_name;
            void* m_addr;
            FunctionSignature m_sig;
    };

    class DataType {
        public:
            struct _Flags {
                unsigned needs_host_construction : 1;
                unsigned is_hidden : 1;
                unsigned _unused : 6;
            };
            enum Flags {
                None = 0,
                HostConstructionNotRequired = 0b00000001,
                IsHidden = 0b00000010
            };

            DataType(IScriptAPI* api, const utils::String& name, u32 size, Primitive primitiveType = Primitive::pt_none);
            DataType(IScriptAPI* api, DataType* retTp, const utils::Array<FunctionArgument>& args, u32 flags);
            ~DataType();

            void setFlags(u32 f);
            void addField(const DataTypeField& field);
            void addMethod(Function* method);
            void addBase(DataType* base);
            void bindEnumValue(const utils::String& name, u32 enumValue);

            IScriptAPI* getApi();
            const utils::String& getName();
            u32 getSize();
            bool isPrimitive();
            bool isFunction();
            Primitive getPrimitiveType();
            FunctionSignature& getSignature();
            _Flags& getFlags();
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
            FunctionSignature m_cbSignature;
            bool m_isFunction;
            _Flags m_flags;
            utils::Array<DataTypeField> m_fields;
            utils::Array<Function*> m_methods;
            utils::Array<DataType*> m_bases;
    };

    struct ScriptNamespace {
        utils::String name;
        utils::Array<DataType*> types;
        utils::Array<Function*> globalFunctions;
        utils::Array<GlobalVariable> globalVars;
    };

    class IScriptAPI : public utils::IWithLogging {
        public:
            IScriptAPI();
            virtual ~IScriptAPI();

            bool initPaths();
            const utils::String& getGameExecutablePath();
            const utils::String& getGameBasePath();
            const utils::String& getScriptEntrypointPath();

            void beginNamespace(const utils::String& name);
            void endNamespace();
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
            std::enable_if_t<std::is_fundamental_v<T> || std::is_same_v<T, pointer> || std::is_same_v<T, void*>, DataType*>
            bind(const utils::String& name);
            
            template <typename T>
            std::enable_if_t<std::is_enum_v<T>, DataType*>
            bind(const utils::String& name);

            template <typename T>
            std::enable_if_t<std::is_base_of_v<IEvent, T> || std::is_same_v<T, IEvent>, void>
            registerEventType(IEventType* eventType, const utils::String& name);

            // Can be called from either thread
            virtual void dispatchEvent(IEvent* event);

            // Should be called from the script thread only
            virtual bool handleEvents(); // Should return true if the event loop should terminate
            virtual bool initialize();
            virtual bool commitBindings();
            virtual bool shutdown();
            virtual bool executeEntry();

            // Should be called from the main thread only
            virtual void signalTermination();
            virtual void signalEventBatchStart();
            virtual void waitForEventBatchCompletion();

        protected:
            utils::Array<ScriptNamespace*> m_namespaces;
            ScriptNamespace* m_currentNamespace;
            robin_hood::unordered_flat_map<size_t, DataType*> m_typeMap;

            std::mutex m_eventMutex;
            std::mutex m_batchMutex;
            std::condition_variable m_batchCondition;
            std::atomic<bool> m_batchFlag;
            std::atomic<bool> m_shouldTerminate;
            utils::Array<IEvent*> m_events;
            utils::Array<IEventType*> m_eventTypes;

            utils::String m_exePath;
            utils::String m_pathBase;
            utils::String m_entryPath;
    };
};