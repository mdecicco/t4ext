#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <core/CActorTypeInfo.h>

namespace t4ext {
    void BindActorTypeInfo(IScriptAPI* api, DataType* tp) {
        tp->bind("actorPath", &CActorTypeInfo::atrPath)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("geomPath", &CActorTypeInfo::mtfPath)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("typeName", &CActorTypeInfo::type)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("activeCount", &CActorTypeInfo::actorCount)->setFlags(DataTypeField::Flags::IsReadOnly);
    }
};