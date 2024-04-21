#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <core/CGame.h>
#include <core/CLevel.h>
#include <core/CActor.h>
#include <core/CActorTypeInfo.h>

namespace t4ext {
    void BindImGui(IScriptAPI* api);
    void BindBuiltins(IScriptAPI* api);
    void BindCGame(IScriptAPI* api, DataType* tp);
    void BindCLevel(IScriptAPI* api, DataType* tp);
    void BindCActor(IScriptAPI* api, DataType* tp);
    void BindActorTypeInfo(IScriptAPI* api, DataType* tp);
    void BindBasicPhysics(IScriptAPI* api, DataType* tp);
    void BindGlobals(IScriptAPI* api);
    void BindSystem(IScriptAPI* api);

    void BindAPI(IScriptAPI* api) {
        api->beginNamespace("");
            BindBuiltins(api);
        api->endNamespace();

        api->beginNamespace("t4");
            // forward declare
            DataType* cg = api->bind<CGame>("CGame");
            DataType* cl = api->bind<CLevel>("CLevel");
            DataType* ca = api->bind<CActor>("CActor");
            DataType* bp = api->bind<CBasicPhysics>("CBasicPhysics");
            DataType* ati = api->bind<CActorTypeInfo>("CActorTypeInfo");

            BindCGame(api, cg);
            BindCLevel(api, cl);
            BindCActor(api, ca);
            BindActorTypeInfo(api, ati);
            BindBasicPhysics(api, bp);
            BindGlobals(api);
        api->endNamespace();

        api->beginNamespace("ImGui");
            BindImGui(api);
        api->endNamespace();

        api->beginNamespace("sys");
            BindSystem(api);
        api->endNamespace();
    }
};