#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>
#include <events/EngineCreate.h>
#include <events/Log.h>

#include <core/CActor.h>

#include <core/CGame.h>
#include <core/AppBase.h>
#include <windows.h>

namespace t4ext {
    void BindGlobals(IScriptAPI* api) {
        api->bind("getEngine", CGame::Get);
        api->bind("sleep", utils::Thread::Sleep)->setArgNames({ "milliseconds" });

        api->bind("addEngineCreateListener", +[](Callback<void, CGame*>& cb) {
            return utils::Singleton<EngineCreateEventType>::Get()->createListener(cb);
        })->setArgNames({ "listener" });

        api->bind("removeEngineCreateListener", +[](u32 listenerId) {
            utils::Singleton<EngineCreateEventType>::Get()->removeListener(listenerId);
        })->setArgNames({ "listenerId" });

        api->bind("getLiveActors", +[]() -> const utils::Array<t4ext::CActor*>& {
            return gClient::Get()->getLiveActors();
        })->setReturnNullable(false);
    }
};