#include <events/ActorCreate.h>
#include <script/IScriptAPI.hpp>
#include <core/CActor.h>

#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

namespace t4ext {
    ActorCreateListener::ActorCreateListener(Callback<void, CActor*>& callback) {
        m_id = 0;
        m_last = m_next = nullptr;
        m_callback = &callback;
    }

    ActorCreateListener::~ActorCreateListener() {
        delete m_callback;
    }

    void ActorCreateListener::execute(CActor* actor) {
        m_callback->call(actor);
    }

    ActorCreateEventType::ActorCreateEventType() {
        m_nextListenerId = 1;
        m_listeners = m_lastListener = nullptr;
    }

    ActorCreateEventType::~ActorCreateEventType() {
        ActorCreateListener* n = m_listeners;
        while (n) {
            ActorCreateListener* next = n->m_next;
            delete n;
            n = next;
        }
        m_listeners = m_lastListener = nullptr;
    }
    
    u32 ActorCreateEventType::createListener(Callback<void, CActor*>& callback) {
        ActorCreateListener* listener = new ActorCreateListener(callback);
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

    void ActorCreateEventType::removeListener(u32 id) {
        auto it = m_listenerMap.find(id);
        if (it == m_listenerMap.end()) return;

        m_listenerMap.erase(it);

        ActorCreateListener* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_listeners == n) m_listeners = n->m_next;
        if (m_lastListener == n) m_lastListener = n->m_last;

        delete n;
    }

    void ActorCreateEventType::notifyListeners(CActor* actor) {
        ActorCreateListener* n = m_listeners;
        while (n) {
            n->execute(actor);
            n = n->m_next;
        }
    }

    bool ActorCreateEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_listeners != nullptr;
    }
    
    ActorCreateEvent* ActorCreateEventType::createEvent(CActor* actor) {
        return new ActorCreateEvent(this, actor);
    }
    
    void ActorCreateEventType::destroyEvent(ActorCreateEvent* event) {
        delete event;
    }
    
    ActorCreateEvent::ActorCreateEvent(ActorCreateEventType* tp, CActor* actor) : IEvent(tp) {
        m_actor = actor;
    }

    ActorCreateEvent::~ActorCreateEvent() {
    }

    void ActorCreateEvent::process(IScriptAPI* api) {
        ((ActorCreateEventType*)m_type)->notifyListeners(m_actor);
        ((ActorCreateEventType*)m_type)->destroyEvent(this);
    }
};