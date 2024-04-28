#include <events/LevelCreate.h>
#include <script/IScriptAPI.hpp>
#include <core/CLevel.h>

#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

namespace t4ext {
    LevelCreateListener::LevelCreateListener(Callback<void, CLevel*>& callback) {
        m_id = 0;
        m_last = m_next = nullptr;
        m_callback = &callback;
    }

    LevelCreateListener::~LevelCreateListener() {
        delete m_callback;
    }

    void LevelCreateListener::execute(CLevel* level) {
        m_callback->call(level);
    }

    LevelCreateEventType::LevelCreateEventType() {
        m_nextListenerId = 1;
        m_listeners = m_lastListener = nullptr;
    }

    LevelCreateEventType::~LevelCreateEventType() {
        LevelCreateListener* n = m_listeners;
        while (n) {
            LevelCreateListener* next = n->m_next;
            delete n;
            n = next;
        }
        m_listeners = m_lastListener = nullptr;
    }
    
    u32 LevelCreateEventType::createListener(Callback<void, CLevel*>& callback) {
        LevelCreateListener* listener = new LevelCreateListener(callback);
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

    void LevelCreateEventType::removeListener(u32 id) {
        auto it = m_listenerMap.find(id);
        if (it == m_listenerMap.end()) return;

        LevelCreateListener* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_listeners == n) m_listeners = n->m_next;
        if (m_lastListener == n) m_lastListener = n->m_last;

        m_listenerMap.erase(it);

        delete n;
    }

    void LevelCreateEventType::notifyListeners(CLevel* level) {
        LevelCreateListener* n = m_listeners;
        while (n) {
            LevelCreateListener* next = n->m_next;
            n->execute(level);
            n = next;
        }
    }

    bool LevelCreateEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_listeners != nullptr;
    }
    
    LevelCreateEvent* LevelCreateEventType::createEvent(CLevel* level) {
        return new LevelCreateEvent(this, level);
    }
    
    void LevelCreateEventType::destroyEvent(LevelCreateEvent* event) {
        delete event;
    }
    
    LevelCreateEvent::LevelCreateEvent(LevelCreateEventType* tp, CLevel* level) : IEvent(tp) {
        m_level = level;
    }

    LevelCreateEvent::~LevelCreateEvent() {
    }

    void LevelCreateEvent::process(IScriptAPI* api) {
        ((LevelCreateEventType*)m_type)->notifyListeners(m_level);
        ((LevelCreateEventType*)m_type)->destroyEvent(this);
    }
};