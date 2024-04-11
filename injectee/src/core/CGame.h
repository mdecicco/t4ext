#pragma once
#include <types.h>

namespace t4ext {
    class CLevel;
    class CActor;

    class CGame {
        public:
            virtual void method1_0x4();
            virtual void method2_0x8();
            virtual void method3_0xc();
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
            virtual void Create();
            virtual CActor* spawnActor(CLevel* level, const char* name, const char* type, const char* mtfPath);
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
            virtual void method32_0x80();
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
            virtual void method44_0xb0();
            virtual void method45_0xb4();
            virtual void method46_0xb8();
            virtual void method47_0xbc();
            virtual void method48_0xc0();
            virtual void method49_0xc4();

            
            CActor* spawnActorOverride(CLevel* level, const char* name, const char* type, const char* mtfPath);

            static CGame* Get();
    };
};