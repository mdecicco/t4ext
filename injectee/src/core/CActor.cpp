#include <core/CActor.h>
#include <events/ActorCollision.h>
#include <utils/Singleton.hpp>

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
        void (__thiscall CActor::*fn)(utils::vec3f);
        memcpy(&fn, &addr, 4);
        (this->*fn)(scale);
    }

    void CActor::setVisibility(bool isVisible) {
        u32 addr = 0x0051e3b0;
        void (CActor::*fn)(i32);
        memcpy(&fn, &addr, 4);
        (this->*fn)(isVisible ? 1 : 0);
    }

    bool CActor::isVisible() {
        return (flags0 & 0b00000000000000000000000001000000) == 1;
    }
    
    CActor* CActor::getPrev() {
        return prev;
    }

    CActor* CActor::getNext() {
        return next;
    }

    u32 CActor::addCollisionListener(CActor* whenCollidingWith, Callback<void, CActor*, CActor*>& cb) {
        return utils::Singleton<ActorCollisionEventType>::Get()->createListener(this, whenCollidingWith, cb);
    }

    void CActor::removeCollisionListener(u32 listenerId) {
        utils::Singleton<ActorCollisionEventType>::Get()->removeListener(listenerId);
    }
};