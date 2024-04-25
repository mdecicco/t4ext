#pragma once
#include <types.h>
#include <script/IScriptAPI.h>

#include <utils/Math.hpp>
#include <utils/Array.h>

namespace t4ext {
    class CActor;
    class CLevel;
    class CCamera;

    struct UnkClass2 {
        f32 field0_0x0;
        f32 field1_0x4;
        undefined4 field2_0x8;
        f32 field3_0xc;
        undefined4 field4_0x10;
        undefined4 field5_0x14;
        undefined4 field6_0x18;
        undefined4 field7_0x1c;
        undefined4 field8_0x20;
        undefined4 field9_0x24;
        undefined4 field10_0x28;
        undefined4 field11_0x2c;
        undefined4 field12_0x30;
        undefined4 field13_0x34;
        undefined4 field14_0x38;
    };
    
    struct UnkClass4 {
        undefined4 field0_0x0;
        f32 field1_0x4;
        undefined4 field2_0x8;
        f32 field3_0xc;
        UnkClass4 *field4_0x10;
        utils::vec3f field5_0x14;
        f32 field6_0x20;
        undefined4 field7_0x24;
        undefined4 field8_0x28;
        undefined4 field9_0x2c;
        UnkClass4 *field10_0x30;
    };
    
    struct UnkClass5 {
        undefined4 field0_0x0;
        f32 gravity;
        undefined4 field2_0x8;
        f32 field3_0xc;
        f32 field4_0x10;
        undefined4 field5_0x14;
        undefined4 field6_0x18;
        char* atrPath;
        char* field8_0x20;
        undefined4 field9_0x24;
        undefined4 field10_0x28;
        char* field11_0x2c;
        UnkClass4 field12_0x30;
        f32 field13_0x64;
        f32 field14_0x68;
        undefined4 field15_0x6c;
        undefined4 field16_0x70;
    };

    struct UnkClass6 {
        CActor *field0_0x0;
        CActor *field1_0x4;
        undefined4 field2_0x8;
        void *field3_0xc;
        void *field4_0x10;
    };

    struct UnkClass9 {
        void* field0_0x0; // It's a function
        undefined4 field1_0x4;
        undefined4 field2_0x8;
        undefined4 field3_0xc;
        undefined4 field4_0x10;
        undefined4 field5_0x14;
        undefined4 field6_0x18;
        undefined4 field7_0x1c;
        undefined4 field8_0x20;
        undefined4 field9_0x24;
        undefined4 field10_0x28;
        undefined4 field11_0x2c;
        undefined4 field12_0x30;
    };

    struct UnkClass8 {
        undefined4 field0_0x0;
        undefined4 field1_0x4;
        undefined4 field2_0x8;
        undefined4 field3_0xc;
        undefined4 field4_0x10;
        undefined4 field5_0x14;
        undefined4 field6_0x18;
        undefined4 field7_0x1c;
        undefined4 field8_0x20;
        undefined4 field9_0x24;
        undefined4 field10_0x28;
        undefined4 field11_0x2c;
    };

    struct UnkClass11 {
        CLevel *level;
        void* field1_0x4; // It's a function
        u32 field2_0x8;
        u32 field3_0xc;
        undefined4 field4_0x10;
    };

    class CLevel {
        public:
            CActor* createActor(const char* type, const char* path);
            CActor* addActor(CActor* actor, i32 p2);
            CActor* spawnActor(CActor* actor, i32 p2);
            CActor* spawnActorAtPosition(i32 p1, const char* type, const char* path, const utils::vec3f& pos, i32 p5);
            utils::Array<CCamera*>* getCameras();

            u32 addActorAddedListener(Callback<void, CActor*>& cb);
            void removeActorAddedListener(u32 listenerId);

            virtual void method_0x0();

            undefined4 field1_0x4;
            undefined4 field2_0x8;
            undefined4 field3_0xc;
            undefined4 field4_0x10;
            undefined4 field5_0x14;
            undefined4 field6_0x18;
            CLevel* field7_0x1c;
            char* atrPath;
            UnkClass5 info;
            char* field10_0x98;
            UnkClass6 field11_0x9c;
            undefined4 field12_0xb0;
            undefined4 field13_0xb4;
            undefined4 field14_0xb8;
            undefined4 field15_0xbc;
            CCamera **camerasBegin;
            CCamera **camerasEnd;
            undefined4 field18_0xc8;
            undefined4 field19_0xcc;
            undefined4 field20_0xd0;
            undefined4 field21_0xd4;
            undefined4 field22_0xd8;
            undefined4 field23_0xdc;
            undefined4 field24_0xe0;
            undefined4 field25_0xe4;
            undefined4 field26_0xe8;
            undefined4 field27_0xec;
            CActor *field28_0xf0;
            CActor *field29_0xf4;
            f32 field30_0xf8;
            UnkClass2 *field31_0xfc;
            f32 *field32_0x100;
            undefined4 field33_0x104;
            undefined4 field34_0x108;
            undefined4 field35_0x10c;
            void* field36_0x110; // it's a function
            f32 field37_0x114;
            undefined4 field38_0x118;
            undefined4 field39_0x11c;
            undefined4 field40_0x120;
            undefined4 field41_0x124;
            f32 field42_0x128;
            UnkClass8 *field43_0x12c;
            CActor* levelActor;
            undefined4 field45_0x134;
            undefined4 field46_0x138;
            undefined4 field47_0x13c;
            undefined4 field48_0x140;
            f32 field49_0x144;
            undefined4 field50_0x148;
            undefined4 field51_0x14c;
            undefined4 field52_0x150;
            undefined4 field53_0x154;
            undefined4 field54_0x158;
            undefined4 field55_0x15c;
            UnkClass9 *field56_0x160;
            UnkClass11 *field57_0x164;
            undefined4 field58_0x168;
            i32 field59_0x16c;
            undefined4 field60_0x170;
            undefined4 field61_0x174;
            i32 field62_0x178;
            undefined4 field63_0x17c;
            undefined4 field64_0x180;
            undefined4 field65_0x184;
            undefined4 field66_0x188;
            undefined4 field67_0x18c;
            undefined field68_0x190;
            undefined field69_0x191;
            undefined field70_0x192;
            undefined field71_0x193;
            undefined field72_0x194;
            undefined field73_0x195;
            undefined field74_0x196;
            undefined field75_0x197;
            undefined field76_0x198;
            undefined field77_0x199;
            undefined field78_0x19a;
            undefined field79_0x19b;
            undefined field80_0x19c;
            undefined field81_0x19d;
            undefined field82_0x19e;
            undefined field83_0x19f;
            undefined field84_0x1a0;
            undefined field85_0x1a1;
            undefined field86_0x1a2;
            undefined field87_0x1a3;
            undefined field88_0x1a4;
            undefined field89_0x1a5;
            undefined field90_0x1a6;
            undefined field91_0x1a7;
            undefined field92_0x1a8;
            undefined field93_0x1a9;
            undefined field94_0x1aa;
            undefined field95_0x1ab;
            undefined field96_0x1ac;
            undefined field97_0x1ad;
            undefined field98_0x1ae;
            undefined field99_0x1af;
            undefined field100_0x1b0;
            undefined field101_0x1b1;
            undefined field102_0x1b2;
            undefined field103_0x1b3;
            undefined field104_0x1b4;
            undefined field105_0x1b5;
            undefined field106_0x1b6;
            undefined field107_0x1b7;
            undefined field108_0x1b8;
            undefined field109_0x1b9;
            undefined field110_0x1ba;
            undefined field111_0x1bb;
            undefined field112_0x1bc;
            undefined field113_0x1bd;
            undefined field114_0x1be;
            undefined field115_0x1bf;
            undefined field116_0x1c0;
            undefined field117_0x1c1;
            undefined field118_0x1c2;
            undefined field119_0x1c3;
            undefined field120_0x1c4;
            undefined field121_0x1c5;
            undefined field122_0x1c6;
            undefined field123_0x1c7;
            undefined field124_0x1c8;
            undefined field125_0x1c9;
            undefined field126_0x1ca;
            undefined field127_0x1cb;
            undefined field128_0x1cc;
            undefined field129_0x1cd;
            undefined field130_0x1ce;
            undefined field131_0x1cf;
            undefined field132_0x1d0;
            undefined field133_0x1d1;
            undefined field134_0x1d2;
            undefined field135_0x1d3;
            undefined field136_0x1d4;
            undefined field137_0x1d5;
            undefined field138_0x1d6;
            undefined field139_0x1d7;
            undefined field140_0x1d8;
            undefined field141_0x1d9;
            undefined field142_0x1da;
            undefined field143_0x1db;
            undefined field144_0x1dc;
            undefined field145_0x1dd;
            undefined field146_0x1de;
            undefined field147_0x1df;
            undefined field148_0x1e0;
            undefined field149_0x1e1;
            undefined field150_0x1e2;
            undefined field151_0x1e3;
            undefined field152_0x1e4;
            undefined field153_0x1e5;
            undefined field154_0x1e6;
            undefined field155_0x1e7;
            undefined field156_0x1e8;
            undefined field157_0x1e9;
            undefined field158_0x1ea;
            undefined field159_0x1eb;
            undefined field160_0x1ec;
            undefined field161_0x1ed;
            undefined field162_0x1ee;
            undefined field163_0x1ef;
            undefined field164_0x1f0;
            undefined field165_0x1f1;
            undefined field166_0x1f2;
            undefined field167_0x1f3;
            undefined field168_0x1f4;
            undefined field169_0x1f5;
            undefined field170_0x1f6;
            undefined field171_0x1f7;
            undefined field172_0x1f8;
            undefined field173_0x1f9;
            undefined field174_0x1fa;
            undefined field175_0x1fb;
            undefined field176_0x1fc;
            undefined field177_0x1fd;
            undefined field178_0x1fe;
            undefined field179_0x1ff;
            undefined field180_0x200;
            undefined field181_0x201;
            undefined field182_0x202;
            undefined field183_0x203;
            undefined field184_0x204;
            undefined field185_0x205;
            undefined field186_0x206;
            undefined field187_0x207;
            undefined field188_0x208;
            undefined field189_0x209;
            undefined field190_0x20a;
            undefined field191_0x20b;
            undefined field192_0x20c;
            undefined field193_0x20d;
            undefined field194_0x20e;
            undefined field195_0x20f;
            undefined field196_0x210;
            undefined field197_0x211;
            undefined field198_0x212;
            undefined field199_0x213;
            undefined field200_0x214;
            undefined field201_0x215;
            undefined field202_0x216;
            undefined field203_0x217;
            undefined field204_0x218;
            undefined field205_0x219;
            undefined field206_0x21a;
            undefined field207_0x21b;
            undefined field208_0x21c;
            undefined field209_0x21d;
            undefined field210_0x21e;
            undefined field211_0x21f;
            undefined field212_0x220;
            undefined field213_0x221;
            undefined field214_0x222;
            undefined field215_0x223;
            undefined field216_0x224;
            undefined field217_0x225;
            undefined field218_0x226;
            undefined field219_0x227;
            undefined field220_0x228;
            undefined field221_0x229;
            undefined field222_0x22a;
            undefined field223_0x22b;
            undefined field224_0x22c;
            undefined field225_0x22d;
            undefined field226_0x22e;
            undefined field227_0x22f;
            undefined field228_0x230;
            undefined field229_0x231;
            undefined field230_0x232;
            undefined field231_0x233;
            undefined field232_0x234;
            undefined field233_0x235;
            undefined field234_0x236;
            undefined field235_0x237;
            undefined field236_0x238;
            undefined field237_0x239;
            undefined field238_0x23a;
            undefined field239_0x23b;
            undefined field240_0x23c;
            undefined field241_0x23d;
            undefined field242_0x23e;
            undefined field243_0x23f;
            undefined field244_0x240;
            undefined field245_0x241;
            undefined field246_0x242;
            undefined field247_0x243;
            undefined field248_0x244;
            undefined field249_0x245;
            undefined field250_0x246;
            undefined field251_0x247;
            undefined field252_0x248;
            undefined field253_0x249;
            undefined field254_0x24a;
            undefined field255_0x24b;
            undefined field256_0x24c;
            undefined field257_0x24d;
            undefined field258_0x24e;
            undefined field259_0x24f;
            undefined field260_0x250;
            undefined field261_0x251;
            undefined field262_0x252;
            undefined field263_0x253;
            undefined field264_0x254;
            undefined field265_0x255;
            undefined field266_0x256;
            undefined field267_0x257;
            undefined field268_0x258;
            undefined field269_0x259;
            undefined field270_0x25a;
            undefined field271_0x25b;
            undefined field272_0x25c;
            undefined field273_0x25d;
            undefined field274_0x25e;
            undefined field275_0x25f;
            undefined field276_0x260;
            undefined field277_0x261;
            undefined field278_0x262;
            undefined field279_0x263;
            undefined field280_0x264;
            undefined field281_0x265;
            undefined field282_0x266;
            undefined field283_0x267;
            undefined field284_0x268;
            undefined field285_0x269;
            undefined field286_0x26a;
            undefined field287_0x26b;
            undefined field288_0x26c;
            undefined field289_0x26d;
            undefined field290_0x26e;
            undefined field291_0x26f;
            undefined field292_0x270;
            undefined field293_0x271;
            undefined field294_0x272;
            undefined field295_0x273;
            undefined field296_0x274;
            undefined field297_0x275;
            undefined field298_0x276;
            undefined field299_0x277;
            undefined field300_0x278;
            undefined field301_0x279;
            undefined field302_0x27a;
            undefined field303_0x27b;
            undefined field304_0x27c;
            undefined field305_0x27d;
            undefined field306_0x27e;
            undefined field307_0x27f;
            undefined field308_0x280;
            undefined field309_0x281;
            undefined field310_0x282;
            undefined field311_0x283;
            undefined field312_0x284;
            undefined field313_0x285;
            undefined field314_0x286;
            undefined field315_0x287;
            undefined field316_0x288;
            undefined field317_0x289;
            undefined field318_0x28a;
            undefined field319_0x28b;
            undefined field320_0x28c;
            undefined field321_0x28d;
            undefined field322_0x28e;
            undefined field323_0x28f;
            undefined field324_0x290;
            undefined field325_0x291;
            undefined field326_0x292;
            undefined field327_0x293;
            undefined field328_0x294;
            undefined field329_0x295;
            undefined field330_0x296;
            undefined field331_0x297;
            undefined field332_0x298;
            undefined field333_0x299;
            undefined field334_0x29a;
            undefined field335_0x29b;
            undefined field336_0x29c;
            undefined field337_0x29d;
            undefined field338_0x29e;
            undefined field339_0x29f;
            undefined field340_0x2a0;
            undefined field341_0x2a1;
            undefined field342_0x2a2;
            undefined field343_0x2a3;
            undefined4 field344_0x2a4;
            undefined field345_0x2a8;
            undefined field346_0x2a9;
            undefined field347_0x2aa;
            undefined field348_0x2ab;
            undefined4 field349_0x2ac;
            undefined4 field350_0x2b0;
            undefined field351_0x2b4;
            undefined field352_0x2b5;
            undefined field353_0x2b6;
            undefined field354_0x2b7;
            undefined field355_0x2b8;
            undefined field356_0x2b9;
            undefined field357_0x2ba;
            undefined field358_0x2bb;
            undefined field359_0x2bc;
            undefined field360_0x2bd;
            undefined field361_0x2be;
            undefined field362_0x2bf;
            undefined4 field363_0x2c0;
            undefined4 field364_0x2c4;
            undefined4 field365_0x2c8;
            undefined field366_0x2cc;
            undefined field367_0x2cd;
            undefined field368_0x2ce;
            undefined field369_0x2cf;
            undefined4 field370_0x2d0;
            undefined4 field371_0x2d4;
            undefined4 field372_0x2d8;
            undefined4 field373_0x2dc;
            undefined4 field374_0x2e0;
            undefined4 field375_0x2e4;
            undefined4 field376_0x2e8;
            undefined4 field377_0x2ec;
    };
};