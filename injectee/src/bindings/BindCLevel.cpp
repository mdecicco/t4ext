#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <core/CGame.h>
#include <core/CLevel.h>
#include <core/CActor.h>

namespace t4ext {
    void BindCLevel(IScriptAPI* api, DataType* tp) {
        tp->bind("atrPath", &CLevel::atrPath)->flags.is_readonly;
        tp->bind("spawnActor", &CLevel::spawnActorAtPosition);
    }
};