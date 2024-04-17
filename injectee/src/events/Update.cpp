#include <events/Update.h>
#include <script/IScriptAPI.hpp>

#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

namespace t4ext {
    UpdateListener::UpdateListener(ICallback<void>& callback) {
        m_id = 0;
        m_last = m_next = nullptr;
        m_callback = &callback;
    }

    UpdateListener::~UpdateListener() {
        delete m_callback;
    }

    void UpdateListener::execute() {
        m_callback->call();
    }

    UpdateEventType::UpdateEventType() : m_allocator(+[](){ return new utils::FixedAllocator<UpdateEvent>(128); }) {
        m_nextListenerId = 1;
        m_listeners = m_lastListener = nullptr;
    }

    UpdateEventType::~UpdateEventType() {
        UpdateListener* n = m_listeners;
        while (n) {
            UpdateListener* next = n->m_next;
            delete n;
            n = next;
        }
        m_listeners = m_lastListener = nullptr;
    }
    
    u32 UpdateEventType::createListener(ICallback<void>& callback) {
        UpdateListener* listener = new UpdateListener(callback);
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

    void UpdateEventType::removeListener(u32 id) {
        auto it = m_listenerMap.find(id);
        if (it == m_listenerMap.end()) return;

        m_listenerMap.erase(it);

        UpdateListener* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_listeners == n) m_listeners = nullptr;
        if (m_lastListener == n) m_lastListener = n->m_last;

        delete n;
    }

    void UpdateEventType::notifyListeners() {
        UpdateListener* n = m_listeners;
        while (n) {
            n->execute();
            n = n->m_next;
        }
    }

    void UpdateEventType::bind(DataType* eventTp) {
        IEventType::bind(eventTp);

        eventTp->addBase(eventTp->getApi()->getType<IEvent>());
    }

    bool UpdateEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_listeners != nullptr;
    }
    
    IEvent* UpdateEventType::createEvent() {
        return new UpdateEvent(this);
    }
    
    void UpdateEventType::destroyEvent(IEvent* event) {
        delete (UpdateEvent*)event;
    }
    
    UpdateEvent::UpdateEvent(UpdateEventType* tp) : IEvent(tp) {
    }

    UpdateEvent::~UpdateEvent() {
    }

    void UpdateEvent::process(IScriptAPI* api) {
        ((UpdateEventType*)m_type)->notifyListeners();
        ((UpdateEventType*)m_type)->destroyEvent(this);
    }
};