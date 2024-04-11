#pragma once
#include <types.h>

#include <utils/Math.hpp>

namespace t4ext {
    class CActor;

    class CLevel {
        public:
            CActor* createActor(const char* type, const char* path);
            CActor* addActor(CActor* actor, i32 p2);
            CActor* spawnActor(CActor* actor, i32 p2);
            CActor* spawnActorAtPosition(i32 p1, const char* type, const char* path, const utils::vec3f& pos, i32 p5);
    };
};