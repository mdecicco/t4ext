#pragma once
#include <types.h>
#include <events/IEvent.h>
#include <utils/robin_hood.h>
#include <utils/Allocator.h>
#include <script/IScriptAPI.h>

namespace t4ext {
    class CActor;

    class ActorCreateListener {
        public:
            ActorCreateListener(Callback<void, CActor*>& callback);
            virtual ~ActorCreateListener();

            void execute(CActor* actor);

        protected:
            friend class ActorCreateEventType;

            u32 m_id;
            ActorCreateListener* m_next;
            ActorCreateListener* m_last;
            Callback<void, CActor*>* m_callback;
    };
    
    class ActorCreateEventType;
    class ActorCreateEvent : public IEvent {
        public:
            ActorCreateEvent(ActorCreateEventType* tp, CActor* actor);
            virtual ~ActorCreateEvent();

            virtual void process(IScriptAPI* api);
        
        protected:
            CActor* m_actor;
    };
    
    class ActorCreateEventType : public IEventType {
        public:
            ActorCreateEventType();
            virtual ~ActorCreateEventType();

            u32 createListener(Callback<void, CActor*>& callback);
            void removeListener(u32 id);
            void notifyListeners(CActor* actor);

            virtual bool canProduceEvents();
            ActorCreateEvent* createEvent(CActor* actor);
            void destroyEvent(ActorCreateEvent* event);
        
        protected:
            u32 m_nextListenerId;
            ActorCreateListener* m_listeners;
            ActorCreateListener* m_lastListener;
            robin_hood::unordered_map<u32, ActorCreateListener*> m_listenerMap;
    };
};