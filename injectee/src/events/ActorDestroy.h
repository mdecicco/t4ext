#pragma once
#include <types.h>
#include <events/IEvent.h>
#include <utils/robin_hood.h>
#include <utils/Allocator.h>
#include <script/IScriptAPI.h>

namespace t4ext {
    class CActor;

    class ActorDestroyListener {
        public:
            ActorDestroyListener(Callback<void, CActor*>& callback);
            virtual ~ActorDestroyListener();

            void execute(CActor* actor);

        protected:
            friend class ActorDestroyEventType;

            u32 m_id;
            ActorDestroyListener* m_next;
            ActorDestroyListener* m_last;
            Callback<void, CActor*>* m_callback;
    };
    
    class ActorDestroyEventType;
    class ActorDestroyEvent : public IEvent {
        public:
            ActorDestroyEvent(ActorDestroyEventType* tp, CActor* actor);
            virtual ~ActorDestroyEvent();

            virtual void process(IScriptAPI* api);
        
        protected:
            CActor* m_actor;
    };
    
    class ActorDestroyEventType : public IEventType {
        public:
            ActorDestroyEventType();
            virtual ~ActorDestroyEventType();

            u32 createListener(Callback<void, CActor*>& callback);
            void removeListener(u32 id);
            void notifyListeners(CActor* actor);

            virtual bool canProduceEvents();
            ActorDestroyEvent* createEvent(CActor* actor);
            void destroyEvent(ActorDestroyEvent* event);
        
        protected:
            u32 m_nextListenerId;
            ActorDestroyListener* m_listeners;
            ActorDestroyListener* m_lastListener;
            robin_hood::unordered_map<u32, ActorDestroyListener*> m_listenerMap;
    };
};