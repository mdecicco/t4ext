#pragma once
#include <types.h>
#include <events/IEvent.h>
#include <utils/Timer.h>
#include <utils/robin_hood.h>
#include <v8.h>

namespace t4ext {
    class ITimeoutData {
        public:
            ITimeoutData(u32 duration, bool doLoop);
            virtual ~ITimeoutData();

            virtual void execute() = 0;

        protected:
            friend class TimeoutEventType;

            u32 m_id;
            u32 m_duration;
            bool m_loop;
            utils::Timer m_tmr;
            ITimeoutData* m_next;
            ITimeoutData* m_last;
    };

    class TimeoutEventType : public IEventType {
        public:
            TimeoutEventType();
            virtual ~TimeoutEventType();

            u32 createTimeout(ITimeoutData* timeout);
            void removeTimeout(u32 id);
            void processTimeouts();

            virtual void bind(DataType* eventTp);
            virtual bool canProduceEvents();
        
        protected:
            u32 m_nextTimeoutId;
            ITimeoutData* m_timeouts;
            ITimeoutData* m_lastTimeout;
            robin_hood::unordered_map<u32, ITimeoutData*> m_timeoutMap;
    };

    class TimeoutEvent : public IEvent {
        public:
            enum class Type {
                Created,
                Destroyed
            };

            TimeoutEvent(u32 timeoutId, Type tp);
            virtual ~TimeoutEvent();

            virtual void process(IScriptAPI* api);

        protected:
            friend class TimeoutEventType;
            u32 m_timeoutId;
            Type m_actionType;
    };
};