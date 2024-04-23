#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <core/CGame.h>
#include <core/CLevel.h>
#include <core/CActor.h>
#include <core/CActorTypeInfo.h>

namespace t4ext {
    void BindCActor(IScriptAPI* api, DataType* tp) {
        tp->bind("getNextActor", &CActor::getNext);
        tp->bind("getPrevActor", &CActor::getPrev);
        tp->bind("actorFlags", &CActor::actorFlags);
        tp->bind("type", &CActor::type)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("unknownFlags0", &CActor::flags0);
        tp->bind("unknownFlags1", &CActor::flags1);
        tp->bind("position", &CActor::position)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("level", &CActor::level)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("nudge", &CActor::nudge);
        tp->bind("name", &CActor::actorName)->setFlags(DataTypeField::Flags::IsReadOnly);
        // typeName is unreliable
        // tp->bind("typeName", &CActor::typeName)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("scale", &CActor::scale)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("geomFilePath", &CActor::geometryFileName)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("unknownFlag", &CActor::unknownFlag);
        tp->bind("rotationQuat", &CActor::rotationQuat)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("rotationEuler", &CActor::rotationEuler)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("physics", &CActor::basicPhysics);
        tp->bind("mode", &CActor::mode);
        tp->bind("transform", &CActor::transform);
        tp->bind("maybeInverseTransform", &CActor::field_0x170);
        tp->bind("typeInfo", &CActor::typeInfo);
        tp->bind("collisionFlags", &CActor::collides);
        tp->bind("touches", &CActor::touches);
        tp->bind("inputRelated", &CActor::inputRelated);

        tp->bind("setPosition", &CActor::setPosition)->setArgNames({ "position" });
        tp->bind("setRotation", &CActor::setRotation)->setArgNames({ "fromEulerAngles" });
        tp->bind("setScale", &CActor::setScale)->setArgNames({ "scale" });
        tp->bind("setVisibility", &CActor::setVisibility)->setArgNames({ "isVisible" });
        tp->bind("isVisible", &CActor::isVisible);
    }
};