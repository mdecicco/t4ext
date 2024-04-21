#include <events/IEvent.h>
#include <script/IScriptAPI.hpp>


namespace t4ext {
    IEventType::IEventType() {
    }

    IEventType::~IEventType() {
    }

    const utils::String& IEventType::getName() const {
        return m_name;
    }

    bool IEventType::canProduceEvents() {
        return false;
    }
    
    void IEventType::setName(const utils::String& name) {
        m_name = name;
    }



    IEvent::IEvent(IEventType* type) {
        m_type = type;

        // this is safe because the string will live longer than this event
        type_name = m_type->getName().c_str();
    }

    IEvent::~IEvent() {
    }
    
    IEventType* IEvent::getType() const {
        return m_type;
    }

    void IEvent::process(IScriptAPI* api) {
    }
};