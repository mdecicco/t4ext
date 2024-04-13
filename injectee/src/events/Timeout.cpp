#include <events/Timeout.h>
#include <script/IScriptAPI.hpp>

#include <Client.h>
#include <utils/Singleton.hpp>

namespace t4ext {
    ITimeoutData::ITimeoutData(u32 duration, bool doLoop) {
        m_id = 0;
        m_duration = duration;
        m_last = m_next = nullptr;
        m_loop = doLoop;
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

        return m_nextTimeoutId++;
    }

    void TimeoutEventType::removeTimeout(u32 id) {
        if (m_lastTimeout && m_lastTimeout->m_id == id) {
            ITimeoutData* n = m_lastTimeout;
            m_lastTimeout->m_last->m_next = nullptr;
            m_lastTimeout = m_lastTimeout->m_last;
            delete n;
        }

        auto it = m_timeoutMap.find(id);
        if (it == m_timeoutMap.end()) return;

        m_timeoutMap.erase(it);

        ITimeoutData* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_timeouts == n) m_timeouts = nullptr;

        delete n;
    }

    void TimeoutEventType::processTimeouts() {
        ITimeoutData* n = m_timeouts;
        while (n) {
            u32 currentMS = n->m_tmr.elapsed() * 1000;

            if (n->m_duration > currentMS) {
                n = n->m_next;
                continue;
            }

            n->execute();

            if (n->m_loop) {
                n->m_tmr.reset();
                n->m_tmr.start();
                n = n->m_next;
                continue;
            }

            ITimeoutData* next = n->m_next;

            if (n->m_last) n->m_last->m_next = n->m_next;
            if (n->m_next) n->m_next->m_last = n->m_last;
            if (m_timeouts == n) m_timeouts = n->m_next;
            if (m_lastTimeout == n) m_lastTimeout = n->m_last;
            delete n;

            n = next;
        }
    }

    void TimeoutEventType::bind(DataType* eventTp) {
        IEventType::bind(eventTp);

        eventTp->addBase(eventTp->getApi()->getType<IEvent>());
        eventTp->bind("timeoutId", &TimeoutEvent::m_timeoutId)->flags.is_readonly = 1;
        eventTp->bind("actionType", &TimeoutEvent::m_actionType)->flags.is_readonly = 1;
    }

    bool TimeoutEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_timeouts != nullptr;
    }
    
    TimeoutEvent::TimeoutEvent(u32 timeoutId, Type tp)
    : IEvent(utils::Singleton<TimeoutEventType>::Get()), m_timeoutId(timeoutId), m_actionType(tp) {
    }

    TimeoutEvent::~TimeoutEvent() {
    }

    void TimeoutEvent::process(IScriptAPI* api) {
    }
};