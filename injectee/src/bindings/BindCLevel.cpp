#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <core/CGame.h>
#include <core/CLevel.h>
#include <core/CCamera.h>
#include <core/CActor.h>

namespace t4ext {
    void BindCLevel(IScriptAPI* api, DataType* tp) {
        DataType* li = api->bind<UnkClass5>("UnkLevelInfo");
        li->bind("actorPath", &UnkClass5::atrPath)->setFlags(DataTypeField::Flags::IsReadOnly);
        li->bind("gravity", &UnkClass5::gravity);
        
        tp->bind("actorPath", &CLevel::atrPath)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("someTimeRemaining", &CLevel::someTimeRemaining);
        tp->bind("someTimeWarp0", &CLevel::someTimeWarp0);
        tp->bind("someTimeWarp1", &CLevel::someTimeWarp1);
        tp->bind("info", &CLevel::info)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("activeCamera", &CLevel::maybeActiveCamera);
        tp->bind("someTimeElapsed", &CLevel::someTimeElapsed);
        tp->bind("levelActor", &CLevel::levelActor)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("isDead", &CLevel::isDead)->setFlags(DataTypeField::Flags::IsReadOnly);

        tp->bind("spawnActor", &CLevel::spawnActorAtPosition);
        tp->bind("getCameras", &CLevel::getCameras)
        ->setSignatureFlags(FunctionSignature::Flags::DeallocateReturnAfterCall)
        ->setReturnNullable(true)
        ->setReturnValueDeallocationCallback(+[](u8* data) {
            delete (utils::Array<CCamera*>*)data;
        });
        tp->bind("getUpdateActors", &CLevel::getUpdateActors)
        ->setSignatureFlags(FunctionSignature::Flags::DeallocateReturnAfterCall)
        ->setReturnNullable(true)
        ->setReturnValueDeallocationCallback(+[](u8* data) {
            delete (utils::Array<CActor*>*)data;
        });
        tp->bind("getActors", &CLevel::getActors)
        ->setSignatureFlags(FunctionSignature::Flags::DeallocateReturnAfterCall)
        ->setReturnNullable(true)
        ->setReturnValueDeallocationCallback(+[](u8* data) {
            delete (utils::Array<CActor*>*)data;
        });
        tp->bind("addActorAddedListener", &CLevel::addActorAddedListener)->setArgNames({ "listener" });
        tp->bind("removeActorAddedListener", &CLevel::removeActorAddedListener)->setArgNames({ "listenerId" });
    }
};