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
        tp->bind("isEnabled", &CActor::isEnabled);
        tp->bind("isVisible", &CActor::maybeIsVisible)->setFlags(DataTypeField::Flags::IsReadOnly);
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
        tp->bind("usingQuatRotation", &CActor::usingQuatRotation);
        tp->bind("prevPosition", &CActor::prevPosition);
        tp->bind("prevVelocity", &CActor::prevVelocity);
        tp->bind("deltaPos", &CActor::deltaPos);
        tp->bind("rotationQuat", &CActor::rotationQuat)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("rotationEuler", &CActor::rotationEuler)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("physics", &CActor::basicPhysics);
        tp->bind("mode", &CActor::mode);
        tp->bind("transform", &CActor::transform);
        tp->bind("maybeInverseTransform", &CActor::maybeInverseTransform);
        tp->bind("typeInfo", &CActor::typeInfo);
        tp->bind("collisionFlags", &CActor::collides);
        tp->bind("touches", &CActor::touches);
        tp->bind("inputRelated", &CActor::inputRelated);
        tp->bind("parent", &CActor::parent)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("childCount", &CActor::childCount)->setFlags(DataTypeField::Flags::IsReadOnly);
        tp->bind("getChildren", &CActor::getChildren)
        ->setSignatureFlags(FunctionSignature::Flags::DeallocateReturnAfterCall)
        ->setReturnValueDeallocationCallback(+[](u8* data){
            delete (utils::Array<CActor*>*)data;
        });

        tp->bind("setPosition", &CActor::setPosition)->setArgNames({ "position" });
        tp->bind("setRotation", &CActor::setRotation)->setArgNames({ "fromEulerAngles" });
        tp->bind("setScale", &CActor::setScale)->setArgNames({ "scale" });
        tp->bind("setVisibility", &CActor::setVisibility)->setArgNames({ "isVisible" });
        // tp->bind("isVisible", &CActor::isVisible);
        tp->bind("addCollisionListener", &CActor::addCollisionListener)->setArgNames({ "whenCollidingWith", "callback" })->setArgNullable(0, true);
        tp->bind("removeCollisionListener", &CActor::removeCollisionListener)->setArgNames({ "listenerId" });
        tp->bind("addUpdateListener", &CActor::addUpdateListener)->setArgNames({ "callback" });
        tp->bind("removeUpdateListener", &CActor::removeUpdateListener)->setArgNames({ "listenerId" });
    }
};