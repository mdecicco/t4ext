#pragma once
#include <types.h>
#include <events/IEvent.h>
#include <utils/robin_hood.h>
#include <utils/Allocator.h>
#include <script/IScriptAPI.h>

namespace t4ext {
    class CLevel;
    class CActor;

    class ActorAddedListener {
        public:
            ActorAddedListener(CLevel* level, Callback<void, CActor*>& callback);
            virtual ~ActorAddedListener();

            void execute(CActor* actor);

        protected:
            friend class ActorAddedEventType;

            u32 m_id;
            ActorAddedListener* m_next;
            ActorAddedListener* m_last;
            CLevel* m_level;
            Callback<void, CActor*>* m_callback;
    };
    
    class ActorAddedEventType;
    class ActorAddedEvent : public IEvent {
        public:
            ActorAddedEvent(ActorAddedEventType* tp, CLevel* level, CActor* actor);
            virtual ~ActorAddedEvent();

            virtual void process(IScriptAPI* api);

            CLevel* level;
            CActor* actor;
    };
    
    class ActorAddedEventType : public IEventType {
        public:
            ActorAddedEventType();
            virtual ~ActorAddedEventType();

            u32 createListener(CLevel* level, Callback<void, CActor*>& callback);
            void removeListener(u32 id);
            void notifyListeners(CLevel* level, CActor* actor);

            virtual bool canProduceEvents();
            ActorAddedEvent* createEvent(CLevel* level, CActor* actor);
            void destroyEvent(ActorAddedEvent* event);
        
        protected:
            u32 m_nextListenerId;
            ActorAddedListener* m_listeners;
            ActorAddedListener* m_lastListener;
            robin_hood::unordered_map<u32, ActorAddedListener*> m_listenerMap;
    };
};