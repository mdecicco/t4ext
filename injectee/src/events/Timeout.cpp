#include <events/Timeout.h>
#include <script/IScriptAPI.hpp>

#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

namespace t4ext {
    ITimeoutData::ITimeoutData(u32 duration, bool doLoop) {
        m_id = 0;
        m_duration = duration;
        m_last = m_next = nullptr;
        m_loop = doLoop;
        m_isDone = false;
        m_tmr.start();
    }

    ITimeoutData::~ITimeoutData() {
    }

    TimeoutEventType::TimeoutEventType() {
        m_nextTimeoutId = 1;
        m_timeouts = m_lastTimeout = nullptr;
    }

    TimeoutEventType::~TimeoutEventType() {
        ITimeoutData* n = m_timeouts;
        while (n) {
            ITimeoutData* next = n->m_next;
            delete n;
            n = next;
        }
        m_timeouts = m_lastTimeout = nullptr;
    }
    
    u32 TimeoutEventType::createTimeout(ITimeoutData* timeout) {
        timeout->m_id = m_nextTimeoutId;
        timeout->m_tmr.start();

        if (m_lastTimeout) {
            m_lastTimeout->m_next = timeout;
            timeout->m_last = m_lastTimeout;
            m_lastTimeout = timeout;
        } else {
            m_timeouts = m_lastTimeout = timeout;
        }

        m_timeoutMap[m_nextTimeoutId] = timeout;
        return m_nextTimeoutId++;
    }

    void TimeoutEventType::removeTimeout(u32 id) {
        auto it = m_timeoutMap.find(id);
        if (it == m_timeoutMap.end()) return;

        m_timeoutMap.erase(it);

        ITimeoutData* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_timeouts == n) m_timeouts = nullptr;
        if (m_lastTimeout == n) m_lastTimeout = n->m_last;

        delete n;
    }

    bool TimeoutEventType::executeTimeout(u32 id) {
        auto it = m_timeoutMap.find(id);
        if (it == m_timeoutMap.end()) return true;

        ITimeoutData* n = it->second;
        n->execute();

        // we have to check if the script removed the timeout
        if (!m_timeoutMap.contains(id)) {
            // it did
            return true;
        }

        if (n->m_isDone) {
            if (n->m_last) n->m_last->m_next = n->m_next;
            if (n->m_next) n->m_next->m_last = n->m_last;
            if (m_timeouts == n) m_timeouts = nullptr;
            if (m_lastTimeout == n) m_lastTimeout = n->m_last;
            delete n;
            m_timeoutMap.erase(it);
            return true;
        }

        return false;
    }

    void TimeoutEventType::processTimeouts() {
        ITimeoutData* n = m_timeouts;
        while (n) {
            u32 currentMS = n->m_tmr.elapsed() * 1000;

            if (n->m_duration > currentMS || n->m_isDone) {
                n = n->m_next;
                continue;
            }

            gClient::Get()->getAPI()->dispatchEvent(createEvent(n->m_id));

            if (n->m_loop) {
                n->m_tmr.reset();
                n->m_tmr.start();
            } else {
                n->m_isDone = true;
            }

            n = n->m_next;
        }
    }

    bool TimeoutEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_timeouts != nullptr;
    }
    
    TimeoutEvent* TimeoutEventType::createEvent(u32 timeoutId) {
        return new TimeoutEvent(timeoutId);
    }
    
    void TimeoutEventType::destroyEvent(TimeoutEvent* event) {
        delete event;
    }
    
    TimeoutEvent::TimeoutEvent(u32 timeoutId) : IEvent(utils::Singleton<TimeoutEventType>::Get()) {
        m_timeoutId = timeoutId;
    }

    TimeoutEvent::~TimeoutEvent() {
    }

    void TimeoutEvent::process(IScriptAPI* api) {
        TimeoutEventType* tp = utils::Singleton<TimeoutEventType>::Get();
        tp->executeTimeout(m_timeoutId);
        tp->destroyEvent(this);
    }
};