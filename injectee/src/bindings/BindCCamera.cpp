#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <core/CCamera.h>

namespace t4ext {
    void BindCCamera(IScriptAPI* api, DataType* tp) {
        tp->addBase(api->getType<CActor>());
        tp->bind("field13_0x240", &CCamera::field13_0x240);
        tp->bind("field14_0x244", &CCamera::field14_0x244);
        tp->bind("field19_0x24c", &CCamera::field19_0x24c);
        tp->bind("field36_0x260", &CCamera::field36_0x260);
        tp->bind("field53_0x274", &CCamera::field53_0x274);
        tp->bind("field70_0x288", &CCamera::field70_0x288);
        tp->bind("field155_0x2e0", &CCamera::field155_0x2e0);
        tp->bind("field184_0x300", &CCamera::field184_0x300);
        tp->bind("field185_0x304", &CCamera::field185_0x304);
        tp->bind("field186_0x308", &CCamera::field186_0x308);
        tp->bind("field203_0x31c", &CCamera::field203_0x31c);
        tp->bind("field204_0x320", &CCamera::field204_0x320);
        tp->bind("field205_0x324", &CCamera::field205_0x324);
        tp->bind("field206_0x328", &CCamera::field206_0x328);
        tp->bind("field399_0x3ec", &CCamera::field399_0x3ec);
    }
};