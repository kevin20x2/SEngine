//
// Created by kevin on 2024/8/17.
//

#ifndef SSCENECOMPONENT_H
#define SSCENECOMPONENT_H
#include "SCoreComponentBase.h"
#include "Maths/Transform.h"
#include "Maths/Vector.h"


class SSceneComponent : public SCoreComponentBase
{
public:

    FVector GetWorldLocation() const;

    FTransform WorldTransform;

    FTransform RelativeTransform;
};



#endif //SSCENECOMPONENT_H
