#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <core/CGame.h>
#include <core/CLevel.h>
#include <core/CActor.h>

namespace t4ext {
    void BindImGui(IScriptAPI* api);
    void BindBuiltins(IScriptAPI* api);
    void BindCGame(IScriptAPI* api, DataType* tp);
    void BindCLevel(IScriptAPI* api, DataType* tp);
    void BindCActor(IScriptAPI* api, DataType* tp);
    void BindGlobals(IScriptAPI* api);

    void BindAPI(IScriptAPI* api) {
        api->beginNamespace("");
            BindBuiltins(api);
        api->endNamespace();

        api->beginNamespace("t4");
            // forward declare
            DataType* cg = api->bind<CGame>("CGame");
            DataType* cl = api->bind<CLevel>("CLevel");
            DataType* ca = api->bind<CActor>("CActor");

            BindCGame(api, cg);
            BindCLevel(api, cl);
            BindCActor(api, ca);
            BindGlobals(api);
        api->endNamespace();

        api->beginNamespace("ImGui");
            BindImGui(api);
        api->endNamespace();
    }
};