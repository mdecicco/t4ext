#pragma once
#include <types.h>
#include <events/IEvent.h>
#include <utils/robin_hood.h>
#include <utils/Allocator.h>
#include <script/IScriptAPI.h>

namespace t4ext {
    class CActor;

    class ActorUpdateListener {
        public:
            ActorUpdateListener(CActor* actor, Callback<void, f32>& callback);
            virtual ~ActorUpdateListener();

            void execute(f32 dt);

        protected:
            friend class ActorUpdateEventType;

            u32 m_id;
            CActor* m_actor;
            ActorUpdateListener* m_next;
            ActorUpdateListener* m_last;
            Callback<void, f32>* m_callback;
    };
    
    class ActorUpdateEventType;
    class ActorUpdateEvent : public IEvent {
        public:
            ActorUpdateEvent(ActorUpdateEventType* tp, CActor* actor, f32 dt);
            virtual ~ActorUpdateEvent();

            virtual void process(IScriptAPI* api);
        
        protected:
            CActor* m_actor;
            f32 m_dt;
    };
    
    class ActorUpdateEventType : public IEventType {
        public:
            ActorUpdateEventType();
            virtual ~ActorUpdateEventType();

            u32 createListener(CActor* actor, Callback<void, f32>& callback);
            void removeListener(u32 id);
            void notifyListeners(CActor* actor, f32 dt);

            virtual bool canProduceEvents();
            ActorUpdateEvent* createEvent(CActor* actor, f32 dt);
            void destroyEvent(ActorUpdateEvent* event);
        
        protected:
            u32 m_nextListenerId;
            ActorUpdateListener* m_listeners;
            ActorUpdateListener* m_lastListener;
            robin_hood::unordered_map<u32, ActorUpdateListener*> m_listenerMap;
    };
};