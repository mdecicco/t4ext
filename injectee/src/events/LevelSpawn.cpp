#include <events/LevelSpawn.h>
#include <script/IScriptAPI.hpp>
#include <core/CLevel.h>

#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

namespace t4ext {
    LevelSpawnListener::LevelSpawnListener(Callback<void, CLevel*>& callback) {
        m_id = 0;
        m_last = m_next = nullptr;
        m_callback = &callback;
    }

    LevelSpawnListener::~LevelSpawnListener() {
        delete m_callback;
    }

    void LevelSpawnListener::execute(CLevel* level) {
        m_callback->call(level);
    }

    LevelSpawnEventType::LevelSpawnEventType() {
        m_nextListenerId = 1;
        m_listeners = m_lastListener = nullptr;
    }

    LevelSpawnEventType::~LevelSpawnEventType() {
        LevelSpawnListener* n = m_listeners;
        while (n) {
            LevelSpawnListener* next = n->m_next;
            delete n;
            n = next;
        }
        m_listeners = m_lastListener = nullptr;
    }
    
    u32 LevelSpawnEventType::createListener(Callback<void, CLevel*>& callback) {
        LevelSpawnListener* listener = new LevelSpawnListener(callback);
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

    void LevelSpawnEventType::removeListener(u32 id) {
        auto it = m_listenerMap.find(id);
        if (it == m_listenerMap.end()) return;

        LevelSpawnListener* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_listeners == n) m_listeners = n->m_next;
        if (m_lastListener == n) m_lastListener = n->m_last;

        m_listenerMap.erase(it);

        delete n;
    }

    void LevelSpawnEventType::notifyListeners(CLevel* level) {
        LevelSpawnListener* n = m_listeners;
        while (n) {
            LevelSpawnListener* next = n->m_next;
            n->execute(level);
            n = next;
        }
    }

    bool LevelSpawnEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_listeners != nullptr;
    }
    
    LevelSpawnEvent* LevelSpawnEventType::createEvent(CLevel* level) {
        return new LevelSpawnEvent(this, level);
    }
    
    void LevelSpawnEventType::destroyEvent(LevelSpawnEvent* event) {
        delete event;
    }
    
    LevelSpawnEvent::LevelSpawnEvent(LevelSpawnEventType* tp, CLevel* level) : IEvent(tp) {
        m_level = level;
    }

    LevelSpawnEvent::~LevelSpawnEvent() {
    }

    void LevelSpawnEvent::process(IScriptAPI* api) {
        ((LevelSpawnEventType*)m_type)->notifyListeners(m_level);
        ((LevelSpawnEventType*)m_type)->destroyEvent(this);
    }
};