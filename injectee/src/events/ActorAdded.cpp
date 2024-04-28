#include <events/ActorAdded.h>
#include <script/IScriptAPI.hpp>
#include <core/CActor.h>

#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

namespace t4ext {
    ActorAddedListener::ActorAddedListener(CLevel* level, Callback<void, CActor*>& callback) {
        m_id = 0;
        m_last = m_next = nullptr;
        m_level = level;
        m_callback = &callback;
    }

    ActorAddedListener::~ActorAddedListener() {
        delete m_callback;
    }

    void ActorAddedListener::execute(CActor* actor) {
        m_callback->call(actor);
    }

    ActorAddedEventType::ActorAddedEventType() {
        m_nextListenerId = 1;
        m_listeners = m_lastListener = nullptr;
    }

    ActorAddedEventType::~ActorAddedEventType() {
        ActorAddedListener* n = m_listeners;
        while (n) {
            ActorAddedListener* next = n->m_next;
            delete n;
            n = next;
        }
        m_listeners = m_lastListener = nullptr;
    }
    
    u32 ActorAddedEventType::createListener(CLevel* level, Callback<void, CActor*>& callback) {
        ActorAddedListener* listener = new ActorAddedListener(level, callback);
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

    void ActorAddedEventType::removeListener(u32 id) {
        auto it = m_listenerMap.find(id);
        if (it == m_listenerMap.end()) return;

        ActorAddedListener* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_listeners == n) m_listeners = n->m_next;
        if (m_lastListener == n) m_lastListener = n->m_last;

        m_listenerMap.erase(it);

        delete n;
    }

    void ActorAddedEventType::notifyListeners(CLevel* level, CActor* actor) {
        ActorAddedListener* n = m_listeners;
        while (n) {
            if (n->m_level == level) n->execute(actor);
            n = n->m_next;
        }
    }

    bool ActorAddedEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_listeners != nullptr;
    }
    
    ActorAddedEvent* ActorAddedEventType::createEvent(CLevel* level, CActor* actor) {
        return new ActorAddedEvent(this, level, actor);
    }
    
    void ActorAddedEventType::destroyEvent(ActorAddedEvent* event) {
        delete event;
    }
    
    ActorAddedEvent::ActorAddedEvent(ActorAddedEventType* tp, CLevel* _level, CActor* _actor) : IEvent(tp) {
        level = _level;
        actor = _actor;
    }

    ActorAddedEvent::~ActorAddedEvent() {
    }

    void ActorAddedEvent::process(IScriptAPI* api) {
        ((ActorAddedEventType*)m_type)->notifyListeners(level, actor);
        ((ActorAddedEventType*)m_type)->destroyEvent(this);
    }
};