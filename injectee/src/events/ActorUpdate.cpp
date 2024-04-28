#include <events/ActorUpdate.h>
#include <script/IScriptAPI.hpp>
#include <core/CActor.h>

#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

namespace t4ext {
    ActorUpdateListener::ActorUpdateListener(CActor* actor, Callback<void, f32>& callback) {
        m_id = 0;
        m_last = m_next = nullptr;
        m_actor = actor;
        m_callback = &callback;
    }

    ActorUpdateListener::~ActorUpdateListener() {
        delete m_callback;
    }

    void ActorUpdateListener::execute(f32 dt) {
        m_callback->call(dt);
    }

    ActorUpdateEventType::ActorUpdateEventType() {
        m_nextListenerId = 1;
        m_listeners = m_lastListener = nullptr;
    }

    ActorUpdateEventType::~ActorUpdateEventType() {
        ActorUpdateListener* n = m_listeners;
        while (n) {
            ActorUpdateListener* next = n->m_next;
            delete n;
            n = next;
        }
        m_listeners = m_lastListener = nullptr;
    }
    
    u32 ActorUpdateEventType::createListener(CActor* actor, Callback<void, f32>& callback) {
        ActorUpdateListener* listener = new ActorUpdateListener(actor, callback);
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

    void ActorUpdateEventType::removeListener(u32 id) {
        auto it = m_listenerMap.find(id);
        if (it == m_listenerMap.end()) return;

        ActorUpdateListener* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_listeners == n) m_listeners = n->m_next;
        if (m_lastListener == n) m_lastListener = n->m_last;

        m_listenerMap.erase(it);

        delete n;
    }

    void ActorUpdateEventType::notifyListeners(CActor* actor, f32 dt) {
        ActorUpdateListener* n = m_listeners;
        while (n) {
            ActorUpdateListener* next = n->m_next;
            if (n->m_actor == actor) n->execute(dt);
            n = next;
        }
    }

    bool ActorUpdateEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_listeners != nullptr;
    }
    
    ActorUpdateEvent* ActorUpdateEventType::createEvent(CActor* actor, f32 dt) {
        return new ActorUpdateEvent(this, actor, dt);
    }
    
    void ActorUpdateEventType::destroyEvent(ActorUpdateEvent* event) {
        delete event;
    }
    
    ActorUpdateEvent::ActorUpdateEvent(ActorUpdateEventType* tp, CActor* actor, f32 dt) : IEvent(tp) {
        m_actor = actor;
        m_dt = dt;
    }

    ActorUpdateEvent::~ActorUpdateEvent() {
    }

    void ActorUpdateEvent::process(IScriptAPI* api) {
        ((ActorUpdateEventType*)m_type)->notifyListeners(m_actor, m_dt);
        ((ActorUpdateEventType*)m_type)->destroyEvent(this);
    }
};