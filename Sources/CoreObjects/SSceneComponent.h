//
// Created by kevin on 2024/8/17.
//

#ifndef SSCENECOMPONENT_H
#define SSCENECOMPONENT_H
#include "SCoreComponentBase.h"
#include "Maths/Transform.h"


class SSceneComponent : public SCoreComponentBase
{
public:
    SSceneComponent():SCoreComponentBase(nullptr) {}

    FTransform GetWorldTransform() const
    {
        return WorldTransform;
    }

    void SetWorldLocation(const FVector & Location)
    {
        WorldTransform.Location = Location;
    }
    void SetRotation(const FQuat & Rotation)
    {
        WorldTransform.Rotation = Rotation;
    }

protected:

    FTransform WorldTransform;

    FTransform RelativeTransform;
};



#endif //SSCENECOMPONENT_H
