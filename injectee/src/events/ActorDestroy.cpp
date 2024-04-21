#include <events/ActorDestroy.h>
#include <script/IScriptAPI.hpp>
#include <core/CActor.h>

#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

namespace t4ext {
    ActorDestroyListener::ActorDestroyListener(Callback<void, CActor*>& callback) {
        m_id = 0;
        m_last = m_next = nullptr;
        m_callback = &callback;
    }

    ActorDestroyListener::~ActorDestroyListener() {
        delete m_callback;
    }

    void ActorDestroyListener::execute(CActor* actor) {
        m_callback->call(actor);
    }

    ActorDestroyEventType::ActorDestroyEventType() {
        m_nextListenerId = 1;
        m_listeners = m_lastListener = nullptr;
    }

    ActorDestroyEventType::~ActorDestroyEventType() {
        ActorDestroyListener* n = m_listeners;
        while (n) {
            ActorDestroyListener* next = n->m_next;
            delete n;
            n = next;
        }
        m_listeners = m_lastListener = nullptr;
    }
    
    u32 ActorDestroyEventType::createListener(Callback<void, CActor*>& callback) {
        ActorDestroyListener* listener = new ActorDestroyListener(callback);
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

    void ActorDestroyEventType::removeListener(u32 id) {
        auto it = m_listenerMap.find(id);
        if (it == m_listenerMap.end()) return;

        m_listenerMap.erase(it);

        ActorDestroyListener* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_listeners == n) m_listeners = nullptr;
        if (m_lastListener == n) m_lastListener = n->m_last;

        delete n;
    }

    void ActorDestroyEventType::notifyListeners(CActor* actor) {
        ActorDestroyListener* n = m_listeners;
        while (n) {
            n->execute(actor);
            n = n->m_next;
        }
    }

    bool ActorDestroyEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_listeners != nullptr;
    }
    
    ActorDestroyEvent* ActorDestroyEventType::createEvent(CActor* actor) {
        return new ActorDestroyEvent(this, actor);
    }
    
    void ActorDestroyEventType::destroyEvent(ActorDestroyEvent* event) {
        delete event;
    }
    
    ActorDestroyEvent::ActorDestroyEvent(ActorDestroyEventType* tp, CActor* actor) : IEvent(tp) {
        m_actor = actor;
    }

    ActorDestroyEvent::~ActorDestroyEvent() {
    }

    void ActorDestroyEvent::process(IScriptAPI* api) {
        ((ActorDestroyEventType*)m_type)->notifyListeners(m_actor);
        ((ActorDestroyEventType*)m_type)->destroyEvent(this);
    }
};