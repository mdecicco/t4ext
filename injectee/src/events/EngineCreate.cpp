#include <events/EngineCreate.h>
#include <script/IScriptAPI.hpp>
#include <core/CGame.h>

#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

namespace t4ext {
    EngineCreateListener::EngineCreateListener(Callback<void, CGame*>& callback) {
        m_id = 0;
        m_last = m_next = nullptr;
        m_callback = &callback;
    }

    EngineCreateListener::~EngineCreateListener() {
        delete m_callback;
    }

    void EngineCreateListener::execute(CGame* engine) {
        m_callback->call(engine);
    }

    EngineCreateEventType::EngineCreateEventType() {
        m_nextListenerId = 1;
        m_listeners = m_lastListener = nullptr;
    }

    EngineCreateEventType::~EngineCreateEventType() {
        EngineCreateListener* n = m_listeners;
        while (n) {
            EngineCreateListener* next = n->m_next;
            delete n;
            n = next;
        }
        m_listeners = m_lastListener = nullptr;
    }
    
    u32 EngineCreateEventType::createListener(Callback<void, CGame*>& callback) {
        EngineCreateListener* listener = new EngineCreateListener(callback);
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

    void EngineCreateEventType::removeListener(u32 id) {
        auto it = m_listenerMap.find(id);
        if (it == m_listenerMap.end()) return;

        EngineCreateListener* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_listeners == n) m_listeners = n->m_next;
        if (m_lastListener == n) m_lastListener = n->m_last;

        m_listenerMap.erase(it);

        delete n;
    }

    void EngineCreateEventType::notifyListeners(CGame* engine) {
        EngineCreateListener* n = m_listeners;
        while (n) {
            EngineCreateListener* next = n->m_next;
            n->execute(engine);
            n = next;
        }
    }

    bool EngineCreateEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_listeners != nullptr;
    }
    
    EngineCreateEvent* EngineCreateEventType::createEvent(CGame* engine) {
        return new EngineCreateEvent(this, engine);
    }
    
    void EngineCreateEventType::destroyEvent(EngineCreateEvent* event) {
        delete event;
    }
    
    EngineCreateEvent::EngineCreateEvent(EngineCreateEventType* tp, CGame* engine) : IEvent(tp) {
        m_engine = engine;
    }

    EngineCreateEvent::~EngineCreateEvent() {
    }

    void EngineCreateEvent::process(IScriptAPI* api) {
        ((EngineCreateEventType*)m_type)->notifyListeners(m_engine);
        ((EngineCreateEventType*)m_type)->destroyEvent(this);
    }
};