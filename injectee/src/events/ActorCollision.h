#pragma once
#include <types.h>
#include <events/IEvent.h>
#include <utils/robin_hood.h>
#include <utils/Allocator.h>
#include <script/IScriptAPI.h>

namespace t4ext {
    class CActor;

    class CollisionListener {
        public:
            CollisionListener(CActor* actorA, CActor* actorB, Callback<void, CActor*, CActor*>& callback);
            virtual ~CollisionListener();

            void execute(CActor* actorA, CActor* actorB);

        protected:
            friend class ActorCollisionEventType;

            u32 m_id;
            CollisionListener* m_next;
            CollisionListener* m_last;
            CActor* m_actorA;
            CActor* m_actorB;
            Callback<void, CActor*, CActor*>* m_callback;
    };
    
    class ActorCollisionEventType;
    class ActorCollisionEvent : public IEvent {
        public:
            ActorCollisionEvent(ActorCollisionEventType* tp, CActor* a, CActor* b);
            virtual ~ActorCollisionEvent();

            virtual void process(IScriptAPI* api);

            CActor* actorA;
            CActor* actorB;
    };
    
    class ActorCollisionEventType : public IEventType {
        public:
            ActorCollisionEventType();
            virtual ~ActorCollisionEventType();

            u32 createListener(CActor* actorA, CActor* actorB, Callback<void, CActor*, CActor*>& callback);
            void removeListener(u32 id);
            void notifyListeners(CActor* actorA, CActor* actorB);

            virtual bool canProduceEvents();
            ActorCollisionEvent* createEvent(CActor* a, CActor* b);
            void destroyEvent(ActorCollisionEvent* event);
        
        protected:
            u32 m_nextListenerId;
            CollisionListener* m_listeners;
            CollisionListener* m_lastListener;
            robin_hood::unordered_map<u32, CollisionListener*> m_listenerMap;
    };
};