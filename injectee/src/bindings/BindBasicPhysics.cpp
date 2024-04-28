#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>
#include <core/CActor.h>
#include <utils/Math.hpp>

namespace t4ext {
    void BindBasicPhysics(IScriptAPI* api, DataType* tp) {
        tp->bind("mass", &CBasicPhysics::mass);
        tp->bind("field_0x4", &CBasicPhysics::field_0x4);
        tp->bind("gravity", &CBasicPhysics::gravity);
        tp->bind("field_0xC", &CBasicPhysics::field_0xC);
        tp->bind("field_0x10", &CBasicPhysics::field_0x10);
        tp->bind("field_0x14", &CBasicPhysics::field_0x14);
        tp->bind("velocity", &CBasicPhysics::velocity);
        tp->bind("field_0x24", &CBasicPhysics::field_0x24);
        tp->bind("field_0x28", &CBasicPhysics::field_0x28);
        tp->bind("field_0x2C", &CBasicPhysics::field_0x2C);
        tp->bind("field_0x30", &CBasicPhysics::field_0x30);
        tp->bind("field_0x34", &CBasicPhysics::field_0x34);
        tp->bind("field_0x38", &CBasicPhysics::field_0x38);
    }
};