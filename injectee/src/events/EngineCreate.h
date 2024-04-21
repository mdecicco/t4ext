#pragma once
#include <types.h>
#include <events/IEvent.h>
#include <utils/robin_hood.h>
#include <utils/Allocator.h>
#include <script/IScriptAPI.h>

namespace t4ext {
    class CGame;

    class EngineCreateListener {
        public:
            EngineCreateListener(Callback<void, CGame*>& callback);
            virtual ~EngineCreateListener();

            void execute(CGame* engine);

        protected:
            friend class EngineCreateEventType;

            u32 m_id;
            EngineCreateListener* m_next;
            EngineCreateListener* m_last;
            Callback<void, CGame*>* m_callback;
    };
    
    class EngineCreateEventType;
    class EngineCreateEvent : public IEvent {
        public:
            EngineCreateEvent(EngineCreateEventType* tp, CGame* engine);
            virtual ~EngineCreateEvent();

            virtual void process(IScriptAPI* api);
        
        protected:
            CGame* m_engine;
    };
    
    class EngineCreateEventType : public IEventType {
        public:
            EngineCreateEventType();
            virtual ~EngineCreateEventType();

            u32 createListener(Callback<void, CGame*>& callback);
            void removeListener(u32 id);
            void notifyListeners(CGame* engine);

            virtual bool canProduceEvents();
            EngineCreateEvent* createEvent(CGame* engine);
            void destroyEvent(EngineCreateEvent* event);
        
        protected:
            u32 m_nextListenerId;
            EngineCreateListener* m_listeners;
            EngineCreateListener* m_lastListener;
            robin_hood::unordered_map<u32, EngineCreateListener*> m_listenerMap;
    };
};