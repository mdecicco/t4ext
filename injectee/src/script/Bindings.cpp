#include <script/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <events/Timeout.h>

#include <core/CGame.h>
#include <core/CLevel.h>
#include <core/CActor.h>
#include <Client.h>

namespace t4ext {
    void BindAPI(IScriptAPI* api) {
        api->initialize();
        
        api->bind<char>("char");
        api->bind<i8>("i8");
        api->bind<i16>("i16");
        api->bind<i32>("i32");
        api->bind<i64>("i64");
        api->bind<u8>("u8");
        api->bind<u16>("u16");
        api->bind<u32>("u32");
        api->bind<u64>("u64");
        api->bind<f32>("f32");
        api->bind<f64>("f64");
        api->bind<pointer>("pointer");

        DataType* v3f = api->bind<utils::vec3f>("vec3f");
        v3f->bind("x", &utils::vec3f::x);
        v3f->bind("y", &utils::vec3f::y);
        v3f->bind("z", &utils::vec3f::z);

        DataType* etpEnum = api->bind<TimeoutEvent::Type>("TimeoutEventAction");
        etpEnum->bindEnumValue("Created", 0);
        etpEnum->bindEnumValue("Deleted", 1);

        DataType* ca = api->bind<CActor>("CActor");
        DataType* cg = api->bind<CGame>("CGame");
        DataType* cl = api->bind<CLevel>("CLevel");

        cg->bind("levelPath", &CGame::levelPath)->flags.is_readonly;
        cg->bind("currentLevel", &CGame::getCurrentLevel);

        cl->bind("atrPath", &CLevel::atrPath)->flags.is_readonly;
        cl->bind("spawnActor", &CLevel::spawnActorAtPosition);
        
        api->bind("getEngine", CGame::Get);

        api->bind("sleep", utils::Thread::Sleep)->setArgNames({ "milliseconds" });
    }
};