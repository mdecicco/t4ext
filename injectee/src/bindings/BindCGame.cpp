#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <core/CGame.h>
#include <core/CLevel.h>
#include <core/CActor.h>

namespace t4ext {
    void BindCGame(IScriptAPI* api, DataType* tp) {
        tp->bind("levelPath", &CGame::levelPath)->flags.is_readonly;
        tp->bind("getCurrentLevel", &CGame::getCurrentLevel);
        tp->bind("getLevels", &CGame::getLevels)
        ->setSignatureFlags(FunctionSignature::Flags::DeallocateReturnAfterCall)
        ->setReturnValueDeallocationCallback(+[](u8* data){
            delete (utils::Array<CLevel*>*)data;
        });

        tp->bind("addUpdateListener", &CGame::addUpdateListener);
        tp->bind("removeUpdateListener", &CGame::removeUpdateListener);
        tp->bind("addRenderListener", &CGame::addRenderListener);
        tp->bind("removeRenderListener", &CGame::removeRenderListener);
        tp->bind("addActorCreateListener", &CGame::addActorCreateListener);
        tp->bind("removeActorCreateListener", &CGame::removeActorCreateListener);
        tp->bind("addActorDestroyListener", &CGame::addActorDestroyListener);
        tp->bind("removeActorDestroyListener", &CGame::removeActorDestroyListener);
        tp->bind("addLevelCreateListener", &CGame::addLevelCreateListener);
        tp->bind("removeLevelCreateListener", &CGame::removeLevelCreateListener);
        tp->bind("addLevelDestroyListener", &CGame::addLevelDestroyListener);
        tp->bind("removeLevelDestroyListener", &CGame::removeLevelDestroyListener);
        tp->bind("addLevelSpawnListener", &CGame::addLevelSpawnListener);
        tp->bind("removeLevelSpawnListener", &CGame::removeLevelSpawnListener);
        tp->bind("disableInput", &CGame::disableInput);
        tp->bind("enableInput", &CGame::enableInput);
    }
};