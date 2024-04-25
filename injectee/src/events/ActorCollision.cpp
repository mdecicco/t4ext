#include <events/ActorCollision.h>
#include <script/IScriptAPI.hpp>
#include <core/CActor.h>

#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

namespace t4ext {
    CollisionListener::CollisionListener(CActor* actorA, CActor* actorB, Callback<void, CActor*, CActor*>& callback) {
        m_id = 0;
        m_last = m_next = nullptr;
        m_actorA = actorA;
        m_actorB = actorB;
        m_callback = &callback;
    }

    CollisionListener::~CollisionListener() {
        delete m_callback;
    }

    void CollisionListener::execute(CActor* actorA, CActor* actorB) {
        m_callback->call(actorA, actorB);
    }

    ActorCollisionEventType::ActorCollisionEventType() {
        m_nextListenerId = 1;
        m_listeners = m_lastListener = nullptr;
    }

    ActorCollisionEventType::~ActorCollisionEventType() {
        CollisionListener* n = m_listeners;
        while (n) {
            CollisionListener* next = n->m_next;
            delete n;
            n = next;
        }
        m_listeners = m_lastListener = nullptr;
    }
    
    u32 ActorCollisionEventType::createListener(CActor* actorA, CActor* actorB, Callback<void, CActor*, CActor*>& callback) {
        CollisionListener* listener = new CollisionListener(actorA, actorB, callback);
        listener->m_id = m_nextListenerId;

        if (m_lastListener) {
            m_lastListener->m_next = listener;
            listener->m_last = m_lastListener;
            m_lastListener = listener;
        } else {
            m_listeners = m_lastListener = listener;
        }

        m_listenerMap[m_nextListenerId] = listener;
        return m_nextListenerId++;
    }

    void ActorCollisionEventType::removeListener(u32 id) {
        auto it = m_listenerMap.find(id);
        if (it == m_listenerMap.end()) return;

        m_listenerMap.erase(it);

        CollisionListener* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_listeners == n) m_listeners = n->m_next;
        if (m_lastListener == n) m_lastListener = n->m_last;

        delete n;
    }

    void ActorCollisionEventType::notifyListeners(CActor* actorA, CActor* actorB) {
        CollisionListener* n = m_listeners;
        while (n) {
            bool doFire = false;
            CActor* listeningOn = nullptr;
            CActor* collidingWith = nullptr;

            if (!n->m_actorA && !n->m_actorB) {
                doFire = true;
                listeningOn = actorA;
                collidingWith = actorB;
            } else if (n->m_actorA == actorA) {
                listeningOn = actorA;
                collidingWith = actorB;
                if (!n->m_actorB || n->m_actorB == collidingWith) doFire = true;
            } else if (n->m_actorA == actorB) {
                listeningOn = actorB;
                collidingWith = actorA;
                if (!n->m_actorB || n->m_actorB == collidingWith) doFire = true;
            } else if (n->m_actorB == actorA) {
                listeningOn = actorB;
                collidingWith = actorA;
                if (!n->m_actorA || n->m_actorA == listeningOn) doFire = true;
            } else if (n->m_actorB == actorB) {
                listeningOn = actorA;
                collidingWith = actorB;
                if (!n->m_actorA || n->m_actorA == listeningOn) doFire = true;
            }

            if (doFire) n->execute(collidingWith, listeningOn);

            n = n->m_next;
        }
    }

    bool ActorCollisionEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_listeners != nullptr;
    }
    
    ActorCollisionEvent* ActorCollisionEventType::createEvent(CActor* a, CActor* b) {
        return new ActorCollisionEvent(this, a, b);
    }
    
    void ActorCollisionEventType::destroyEvent(ActorCollisionEvent* event) {
        delete event;
    }
    
    ActorCollisionEvent::ActorCollisionEvent(ActorCollisionEventType* tp, CActor* a, CActor* b) : IEvent(tp) {
        actorA = a;
        actorB = b;
    }

    ActorCollisionEvent::~ActorCollisionEvent() {
    }

    void ActorCollisionEvent::process(IScriptAPI* api) {
        ((ActorCollisionEventType*)m_type)->notifyListeners(actorA, actorB);
        ((ActorCollisionEventType*)m_type)->destroyEvent(this);
    }
};