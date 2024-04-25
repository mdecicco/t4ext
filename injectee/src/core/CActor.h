#pragma once
#include <types.h>
#include <utils/Math.hpp>
#include <script/IScriptAPI.h>

namespace t4ext {
    class CLevel;

    struct CBasicPhysics {
        f32 mass;
        f32 field_0x4;
        f32 gravity;
        f32 field_0xC;
        f32 field_0x10;
        f32 field_0x14;
        utils::vec3f velocity;
        f32 field_0x24;
        f32 field_0x28;
        f32 field_0x2C;
        f32 field_0x30;
        f32 field_0x34;
        f32 field_0x38;
    };

    class CActorTypeInfo;

    class CActor {
        public:
            virtual void method0_0x0();
            virtual void method1_0x4();
            virtual void method2_0x8();
            virtual void processInput();
            virtual void method4_0x10();
            virtual void method5_0x14();
            virtual void method6_0x18();
            virtual void method7_0x1c();
            virtual void method8_0x20();
            virtual void method9_0x24();
            virtual void method10_0x28();
            virtual void method11_0x2c();
            virtual void method12_0x30();
            virtual void method13_0x34();
            virtual void method14_0x38();
            virtual void method15_0x3c();
            virtual void method16_0x40();
            virtual void method17_0x44();
            virtual void method18_0x48();
            virtual void method19_0x4c();
            virtual void method20_0x50();
            virtual void method21_0x54();
            virtual void method22_0x58();
            virtual void method23_0x5c();
            virtual void method24_0x60();
            virtual void method25_0x64();
            virtual void method26_0x68();
            virtual void method27_0x6c();
            virtual void method28_0x70();
            virtual void method29_0x74();
            virtual void method30_0x78();
            virtual void method31_0x7c();
            virtual void maybeOnCollide(CActor* collisionWith, i32 p2, i32* p3);
            virtual void method33_0x84();
            virtual void method34_0x88();
            virtual void method35_0x8c();
            virtual void method36_0x90();
            virtual void method37_0x94();
            virtual void method38_0x98();
            virtual void method39_0x9c();
            virtual void method40_0xa0();
            virtual void method41_0xa4();
            virtual void method42_0xa8();
            virtual void method43_0xac();
            virtual void ParseProperty();
            virtual void method45_0xb4();
            virtual void method46_0xb8();
            virtual void method47_0xbc();
            virtual void method48_0xc0();
            virtual void method49_0xc4();
            virtual void method50_0xc8();
            virtual void method51_0xcc();
            virtual void method52_0xd0();
            virtual void method53_0xd4();
            virtual void method54_0xd8();
            virtual void method55_0xdc();
            virtual void method56_0xe0();
            virtual void method57_0xe4();
            virtual void method58_0xe8();
            virtual void method59_0xec();
            virtual void method60_0xf0();
            virtual void method61_0xf4();
            virtual void method62_0xf8();
            virtual void method63_0xfc();
            virtual void method64_0x100();
            virtual void method65_0x104();
            virtual void method66_0x108();
            virtual void method67_0x10c();
            virtual void method68_0x110();
            virtual void method69_0x114();
            virtual void method70_0x118();
            virtual void method71_0x11c();
            virtual void method72_0x120();
            virtual void method73_0x124();
            virtual void method74_0x128();
            virtual void method75_0x12c();
            virtual void method76_0x130();
            virtual void method77_0x134();
            virtual void method78_0x138();
            virtual void method79_0x13c();
            virtual void method80_0x140();
            virtual void method81_0x144();
            virtual void method82_0x148();
            virtual void method83_0x14c();
            virtual void method84_0x150();
            virtual void method85_0x154();
            virtual void destructor();
            virtual void method87_0x15c();
            virtual void method88_0x160();
            virtual void method89_0x164();
            virtual void method90_0x168();
            virtual void method91_0x16c();

            void setPosition(const utils::vec3f& pos);
            void setRotation(const utils::vec3f& euler);
            void setScale(const utils::vec3f& scale);
            void setVisibility(bool isVisible);
            bool isVisible();
            CActor* getPrev();
            CActor* getNext();
            u32 addCollisionListener(CActor* whenCollidingWith, Callback<void, CActor*, CActor*>& cb);
            void removeCollisionListener(u32 listenerId);

            undefined4 field_0x4;
            undefined4 field_0x8;
            undefined4 field_0xC;
            undefined4 field_0x10;
            CActor* next;
            CActor* prev;
            undefined field_0x1C;
            undefined field_0x1D;
            bool isEnabled;
            undefined field_0x1F;
            undefined field_0x20;
            undefined field_0x21;
            undefined field_0x22;
            u8 actorFlags;
            u32 type;
            undefined4 field_0x28;
            undefined4 field_0x2C;
            u32 flags0;
            u32 flags1;
            utils::vec4f position;
            CLevel* level;
            undefined field_0x4C;
            undefined field_0x4D;
            undefined field_0x4E;
            undefined field_0x4F;
            undefined field_0x50;
            u8 nudge;
            undefined field_0x52;
            undefined field_0x53;
            undefined field_0x54;
            undefined field_0x55;
            undefined field_0x56;
            undefined field_0x57;
            f32 field_0x58;
            char* actorName;
            char* typeName;
            undefined field_0x64;
            undefined field_0x65;
            undefined field_0x66;
            undefined field_0x67;
            undefined field_0x68;
            undefined field_0x69;
            undefined field_0x6A;
            undefined field_0x6B;
            undefined field_0x6C;
            undefined field_0x6D;
            undefined field_0x71;
            undefined field_0x72;
            undefined field_0x73;
            undefined4 field_0x74;
            undefined field_0x78;
            undefined field_0x79;
            undefined field_0x7A;
            undefined field_0x7B;
            utils::vec3f scale;
            char* geometryFileName;
            undefined4 field_0x8C;
            undefined4 field_0x90;
            undefined4 field_0x94;
            undefined4 field_0x98;
            undefined4 field_0x9C;
            undefined4 field_0xA0;
            undefined4 field_0xA4;
            undefined4 field_0xA8;
            undefined4 field_0xAC;
            undefined4 field_0xB0;
            undefined4 field_0xB4;
            undefined4 field_0xB8;
            undefined4 field_0xBC;
            undefined4 field_0xC0;
            undefined4 field_0xC4;
            undefined4 field_0xC8;
            undefined4 field_0xCC;
            undefined4 field_0xD0;
            undefined4 field_0xD4;
            undefined4 field_0xD8;
            u32 unknownFlag;
            utils::quatf rotationQuat;
            utils::vec3f rotationEuler;
            CBasicPhysics* basicPhysics;
            undefined4 field_0x100;
            utils::vec3f field_0x104;
            undefined4 field_0x110;
            undefined4 field_0x114;
            undefined4 field_0x118;
            undefined4 field_0x11C;
            undefined4 field_0x120;
            undefined4 field_0x124;
            undefined4 field_0x128;
            i32 mode;
            utils::mat4f transform;
            utils::mat4f field_0x170; // possibly inverse transform
            undefined4 field_0x1B0;
            undefined4 field_0x1B4;
            undefined4 field_0x1B8;
            undefined4 field_0x1BC;
            undefined4 field_0x1C0;
            undefined4 field_0x1C4;
            undefined4 field_0x1C8;
            undefined4 field_0x1CC;
            undefined4 field_0x1D0;
            undefined4 field_0x1D4;
            undefined4 field_0x1D8;
            undefined4 field_0x1DC;
            undefined4 field_0x1E0;
            CActorTypeInfo* typeInfo;
            undefined* actorHealthObject;
            undefined4 field_0x1EC;
            undefined4 field_0x1F0;
            undefined4 field_0x1F4;
            undefined4 field_0x1F8;
            undefined4 field_0x1FC;
            undefined4 field_0x200;
            undefined4 collides;
            undefined4 touches;
            undefined4 ignores;
            undefined4 inputRelated;
            undefined4 field_0x214;
            undefined4 field_0x218;
            undefined4 field_0x21c;
            undefined4 field_0x220;
            undefined4 field_0x224;
            undefined4 field_0x228;
            undefined4 field_0x22c;
            undefined4 field_0x230;
    };
    static_assert(sizeof(CActor) == 0x234, "sizeof(CActor) != 0x234");
};