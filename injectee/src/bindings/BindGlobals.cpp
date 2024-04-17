#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <core/CGame.h>

namespace t4ext {
    void BindGlobals(IScriptAPI* api) {
        api->bind("getEngine", CGame::Get);
        api->bind("sleep", utils::Thread::Sleep)->setArgNames({ "milliseconds" });
    }
};