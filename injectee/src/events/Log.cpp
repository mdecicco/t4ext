#include <events/Log.h>
#include <script/IScriptAPI.hpp>

#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

namespace t4ext {
    LogListener::LogListener(Callback<void, LogEvent*>& callback) {
        m_id = 0;
        m_last = m_next = nullptr;
        m_callback = &callback;
    }

    LogListener::~LogListener() {
        delete m_callback;
    }

    void LogListener::execute(LogEvent* log) {
        m_callback->call(log);
    }

    LogEventType::LogEventType() {
        m_nextListenerId = 1;
        m_listeners = m_lastListener = nullptr;
    }

    LogEventType::~LogEventType() {
        LogListener* n = m_listeners;
        while (n) {
            LogListener* next = n->m_next;
            delete n;
            n = next;
        }
        m_listeners = m_lastListener = nullptr;
    }
    
    u32 LogEventType::createListener(Callback<void, LogEvent*>& callback) {
        LogListener* listener = new LogListener(callback);
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

    void LogEventType::removeListener(u32 id) {
        auto it = m_listenerMap.find(id);
        if (it == m_listenerMap.end()) return;

        m_listenerMap.erase(it);

        LogListener* n = it->second;
        if (n->m_last) n->m_last->m_next = n->m_next;
        if (n->m_next) n->m_next->m_last = n->m_last;
        if (m_listeners == n) m_listeners = nullptr;
        if (m_lastListener == n) m_lastListener = n->m_last;

        delete n;
    }

    void LogEventType::notifyListeners(LogEvent* log) {
        LogListener* n = m_listeners;
        while (n) {
            n->execute(log);
            n = n->m_next;
        }
    }

    void LogEventType::bind() {
        IScriptAPI* api = gClient::Get()->getAPI();
        api->beginNamespace("t4");

        DataType* lv = api->bind<utils::LOG_LEVEL>("LogLevel");
        lv->bindEnumValue("Info", utils::LOG_LEVEL::LOG_INFO);
        lv->bindEnumValue("Warning", utils::LOG_LEVEL::LOG_WARNING);
        lv->bindEnumValue("Error", utils::LOG_LEVEL::LOG_ERROR);
        lv->bindEnumValue("Fatal", utils::LOG_LEVEL::LOG_FATAL);

        DataType* evt = api->bind<LogEvent>("LogEvent");
        evt->bind("level", &LogEvent::m_level)->setFlags(DataTypeField::Flags::IsReadOnly);
        evt->bind("scope", &LogEvent::m_scope)->setFlags(DataTypeField::Flags::IsReadOnly);
        evt->bind("message", &LogEvent::m_message)->setFlags(DataTypeField::Flags::IsReadOnly);

        api->bind("addLogListener", +[](Callback<void, LogEvent*>& cb) {
            return utils::Singleton<LogEventType>::Get()->createListener(cb);
        })->setArgNames({ "listener" });

        api->bind("removeLogListener", +[](u32 listenerId) {
            utils::Singleton<LogEventType>::Get()->removeListener(listenerId);
        })->setArgNames({ "listenerId" });
        api->endNamespace();
    }

    bool LogEventType::canProduceEvents() {
        // If there are no active timeouts, there is no chance of any
        // timeout events occurring
        return m_listeners != nullptr;
    }
    
    LogEvent* LogEventType::createEvent(utils::LOG_LEVEL level, const utils::String& scope, const utils::String& message) {
        return new LogEvent(this, level, scope, message);
    }
    
    void LogEventType::destroyEvent(LogEvent* event) {
        delete event;
    }
    
    LogEvent::LogEvent(LogEventType* tp, utils::LOG_LEVEL level, const utils::String& scope, const utils::String& message) : IEvent(tp) {
        m_level = level;

        if (scope.size() > 0) {
            m_scope = new char[scope.size() + 1];
            strncpy(m_scope, scope.c_str(), scope.size());
            m_scope[scope.size()] = 0;
        } else m_scope = nullptr;

        if (message.size() > 0) {
            m_message = new char[message.size() + 1];
            strncpy(m_message, message.c_str(), message.size());
            m_message[message.size()] = 0;
        } else m_message = nullptr;
    }

    LogEvent::~LogEvent() {
        if (m_scope) delete [] m_scope;
        if (m_message) delete [] m_message;
    }

    void LogEvent::process(IScriptAPI* api) {
        ((LogEventType*)m_type)->notifyListeners(this);
        ((LogEventType*)m_type)->destroyEvent(this);
    }
};