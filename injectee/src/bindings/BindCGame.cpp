#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <core/CGame.h>
#include <core/CLevel.h>
#include <core/CActor.h>

namespace t4ext {
    void BindCGame(IScriptAPI* api, DataType* tp) {
        tp->bind("levelPath", &CGame::levelPath)->flags.is_readonly;
        tp->bind("currentLevel", &CGame::getCurrentLevel);
        tp->bind("addUpdateListener", &CGame::addUpdateListener);
        tp->bind("removeUpdateListener", &CGame::removeUpdateListener);
        tp->bind("addRenderListener", &CGame::addRenderListener);
        tp->bind("removeRenderListener", &CGame::removeRenderListener);
        tp->bind("disableInput", &CGame::disableInput);
        tp->bind("enableInput", &CGame::enableInput);
    }
};