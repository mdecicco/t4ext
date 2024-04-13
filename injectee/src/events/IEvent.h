#pragma once
#include <types.h>
#include <utils/String.h>

namespace t4ext {
    class IScriptAPI;
    class DataType;

    class IEventType {
        public:
            IEventType();
            virtual ~IEventType();

            const utils::String& getName() const;

            virtual void bind(DataType* eventTp);
            virtual bool canProduceEvents();
        
        protected:
            friend class IScriptAPI;
            void setName(const utils::String& name);

            utils::String m_name;
    };

    class IEvent {
        public:
            IEvent(IEventType* type);
            virtual ~IEvent();

            IEventType* getType() const;

            virtual void process(IScriptAPI* api);

        protected:
            friend class IEventType;
            IEventType* m_type;

            // for binding purposes
            const char* type_name;
    };
};