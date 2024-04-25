#include <events/LevelDestroy.h>
#include <script/IScriptAPI.hpp>
#include <core/CLevel.h>

#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

namespace t4ext {
    LevelDestroyListener::LevelDestroyListener(Callback<void, CLevel*>& callback) {
        m_id = 0;
        m_last = m_next = nullptr;
        m_callback = &callback;
    }

    LevelDestroyListener::~LevelDestroyListener() {
        delete m_callback;
    }

    void LevelDestroyListener::execute(CLevel* level) {
        m_callback->call(level);
    }

    LevelDestroyEventType::LevelDestroyEventType() {
        m_nextListenerId = 1;
        m_listeners = m_lastListener = nullptr;
    }

    LevelDestroyEventType::~LevelDestroyEventType() {
        LevelDestroyListener* n = m_listeners;
        while (n) {
            LevelDestroyListener* next = n->m_next;
            delete n;
            n = next;
        }
        m_listeners = m_lastListener = nullptr;
    }
    
    u32 LevelDestroyEventType::createListener(Callback<void, CLevel*>& callback) {
        LevelDestroyListener* listener = new LevelDestroyListener(callback);
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

    void LevelDestroyEventType::removeListener(u32 id) {
        auto it = m_listenerMap.find(id);
        if (it == m_listenerMap.end()) return;

        m_listenerMap.erase(it);

        LevelDestroyListener* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_listeners == n) m_listeners = n->m_next;
        if (m_lastListener == n) m_lastListener = n->m_last;

        delete n;
    }

    void LevelDestroyEventType::notifyListeners(CLevel* level) {
        LevelDestroyListener* n = m_listeners;
        while (n) {
            n->execute(level);
            n = n->m_next;
        }
    }

    bool LevelDestroyEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_listeners != nullptr;
    }
    
    LevelDestroyEvent* LevelDestroyEventType::createEvent(CLevel* level) {
        return new LevelDestroyEvent(this, level);
    }
    
    void LevelDestroyEventType::destroyEvent(LevelDestroyEvent* event) {
        delete event;
    }
    
    LevelDestroyEvent::LevelDestroyEvent(LevelDestroyEventType* tp, CLevel* level) : IEvent(tp) {
        m_level = level;
    }

    LevelDestroyEvent::~LevelDestroyEvent() {
    }

    void LevelDestroyEvent::process(IScriptAPI* api) {
        ((LevelDestroyEventType*)m_type)->notifyListeners(m_level);
        ((LevelDestroyEventType*)m_type)->destroyEvent(this);
    }
};