#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <utils/Math.hpp>

namespace t4ext {
    void BindBuiltins(IScriptAPI* api) {
        api->bind<bool>("boolean")->setFlags(DataType::Flags::IsHidden);
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

        DataType* v2f = api->bind<utils::vec2f>("vec2f");
        v2f->setFlags(DataType::Flags::HostConstructionNotRequired | DataType::Flags::IsHidden);
        v2f->bind("x", &utils::vec2f::x);
        v2f->bind("y", &utils::vec2f::y);

        DataType* v3f = api->bind<utils::vec3f>("vec3f");
        v3f->setFlags(DataType::Flags::HostConstructionNotRequired | DataType::Flags::IsHidden);
        v3f->bind("x", &utils::vec3f::x);
        v3f->bind("y", &utils::vec3f::y);
        v3f->bind("z", &utils::vec3f::z);

        DataType* v4f = api->bind<utils::vec4f>("vec4f");
        v4f->setFlags(DataType::Flags::HostConstructionNotRequired | DataType::Flags::IsHidden);
        v4f->bind("x", &utils::vec4f::x);
        v4f->bind("y", &utils::vec4f::y);
        v4f->bind("z", &utils::vec4f::z);
        v4f->bind("w", &utils::vec4f::w);

        DataType* v2i = api->bind<utils::vec2i>("vec2i");
        v2i->setFlags(DataType::Flags::HostConstructionNotRequired | DataType::Flags::IsHidden);
        v2i->bind("x", &utils::vec2i::x);
        v2i->bind("y", &utils::vec2i::y);

        DataType* v3i = api->bind<utils::vec3i>("vec3i");
        v3i->setFlags(DataType::Flags::HostConstructionNotRequired | DataType::Flags::IsHidden);
        v3i->bind("x", &utils::vec3i::x);
        v3i->bind("y", &utils::vec3i::y);
        v3i->bind("z", &utils::vec3i::z);

        DataType* v4i = api->bind<utils::vec4i>("vec4i");
        v4i->setFlags(DataType::Flags::HostConstructionNotRequired | DataType::Flags::IsHidden);
        v4i->bind("x", &utils::vec4i::x);
        v4i->bind("y", &utils::vec4i::y);
        v4i->bind("z", &utils::vec4i::z);
        v4i->bind("w", &utils::vec4i::w);

        DataType* m3f = api->bind<utils::mat3f>("mat3f");
        m3f->setFlags(DataType::Flags::HostConstructionNotRequired | DataType::Flags::IsHidden);
        m3f->bind("x", &utils::mat3f::x);
        m3f->bind("y", &utils::mat3f::y);
        m3f->bind("z", &utils::mat3f::z);

        DataType* m4f = api->bind<utils::mat4f>("mat4f");
        m4f->setFlags(DataType::Flags::HostConstructionNotRequired | DataType::Flags::IsHidden);
        m4f->bind("x", &utils::mat4f::x);
        m4f->bind("y", &utils::mat4f::y);
        m4f->bind("z", &utils::mat4f::z);
        m4f->bind("w", &utils::mat4f::w);
    }
};