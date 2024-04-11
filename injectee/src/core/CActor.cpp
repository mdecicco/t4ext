#include <core/CActor.h>

#include <string.h>

namespace t4ext {
    void CActor::setRotation(const utils::vec3f& euler) {
        u32 addr = 0x00514490;
        void (CActor::*fn)(const utils::vec3f&);
        memcpy(&fn, &addr, 4);
        (this->*fn)(euler);
    }

    void CActor::setPosition(const utils::vec3f& pos) {
        u32 addr = 0x0051a4d0;
        void (CActor::*fn)(const utils::vec3f&);
        memcpy(&fn, &addr, 4);
        (this->*fn)(pos);
    }
    
    void CActor::setVisibility(bool isVisible) {
        u32 addr = 0x0051e3b0;
        void (CActor::*fn)(i32);
        memcpy(&fn, &addr, 4);
        (this->*fn)(isVisible ? 1 : 0);
    }

    void CActor::setCollides(undefined4 value) {
        m_collides = value;
    }

    void CActor::setTouches(undefined4 value) {
        m_touches = value;
    }

    void CActor::setIgnores(undefined4 value) {
        m_ignores = value;
    }

    CLevel* CActor::getLevel() {
        return m_level;
    }

    const char* CActor::getName() {
        return m_actorName;
    }

    const char* CActor::getTypeName() {
        return m_typeName;
    }

    CActor* CActor::getPrev() {
        return m_prev;
    }

    CActor* CActor::getNext() {
        return m_next;
    }

    utils::vec3f& CActor::getPosition() {
        return m_position;
    }

    utils::vec3f& CActor::getScale() {
        return m_scale;
    }

    utils::quatf& CActor::getRotation() {
        return m_rotation;
    }

    utils::mat4f& CActor::getTransform() {
        return m_transform;
    }
};