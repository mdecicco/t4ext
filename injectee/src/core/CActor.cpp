#include <core/CActor.h>

#include <string.h>

namespace t4ext {
    void CActor::setPosition(const utils::vec3f& pos) {
        u32 addr = 0x0051a4d0;
        void (CActor::*fn)(const utils::vec3f&);
        memcpy(&fn, &addr, 4);
        (this->*fn)(pos);
    }
    
    void CActor::setRotation(const utils::vec3f& euler) {
        u32 addr = 0x00514490;
        void (CActor::*fn)(const utils::vec3f&);
        memcpy(&fn, &addr, 4);
        (this->*fn)(euler);
    }

    void CActor::setScale(const utils::vec3f& scale) {
        u32 addr = 0x0051a590;
        void (CActor::*fn)(const utils::vec3f&);
        memcpy(&fn, &addr, 4);
        (this->*fn)(scale);
    }

    void CActor::setVisibility(bool isVisible) {
        u32 addr = 0x0051e3b0;
        void (CActor::*fn)(i32);
        memcpy(&fn, &addr, 4);
        (this->*fn)(isVisible ? 1 : 0);
    }

    void CActor::setCollides(undefined4 value) {
        collides = value;
    }

    void CActor::setTouches(undefined4 value) {
        touches = value;
    }

    void CActor::setIgnores(undefined4 value) {
        ignores = value;
    }

    CLevel* CActor::getLevel() {
        return level;
    }

    const char* CActor::getName() {
        return actorName;
    }

    const char* CActor::getTypeName() {
        return typeName;
    }

    CActor* CActor::getPrev() {
        return prev;
    }

    CActor* CActor::getNext() {
        return next;
    }

    utils::vec4f& CActor::getPosition() {
        return position;
    }

    utils::vec3f& CActor::getScale() {
        return scale;
    }

    utils::quatf& CActor::getRotation() {
        return rotation;
    }

    utils::mat4f& CActor::getTransform() {
        return transform;
    }
};