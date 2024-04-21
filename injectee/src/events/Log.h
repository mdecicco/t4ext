#pragma once
#include <types.h>
#include <events/IEvent.h>
#include <utils/robin_hood.h>
#include <utils/Allocator.h>
#include <script/IScriptAPI.h>

namespace t4ext {
    class LogEvent;

    class LogListener {
        public:
            LogListener(Callback<void, LogEvent*>& callback);
            virtual ~LogListener();

            void execute(LogEvent* log);

        protected:
            friend class LogEventType;

            u32 m_id;
            LogListener* m_next;
            LogListener* m_last;
            Callback<void, LogEvent*>* m_callback;
    };
    
    class LogEventType;
    class LogEvent : public IEvent {
        public:
            LogEvent(LogEventType* tp, utils::LOG_LEVEL level, const utils::String& scope, const utils::String& message);
            virtual ~LogEvent();

            virtual void process(IScriptAPI* api);         

        protected:
            friend class LogEventType;
            char* m_message;
            char* m_scope;
            utils::LOG_LEVEL m_level;
    };
    
    class LogEventType : public IEventType {
        public:
            LogEventType();
            virtual ~LogEventType();

            u32 createListener(Callback<void, LogEvent*>& callback);
            void removeListener(u32 id);
            void notifyListeners(LogEvent* log);

            void bind();
            virtual bool canProduceEvents();
            LogEvent* createEvent(utils::LOG_LEVEL level, const utils::String& scope, const utils::String& message);
            void destroyEvent(LogEvent* event);
        
        protected:
            u32 m_nextListenerId;
            LogListener* m_listeners;
            LogListener* m_lastListener;
            robin_hood::unordered_map<u32, LogListener*> m_listenerMap;
    };
};