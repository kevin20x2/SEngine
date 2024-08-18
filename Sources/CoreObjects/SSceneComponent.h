//
// Created by kevin on 2024/8/17.
//

#ifndef SSCENECOMPONENT_H
#define SSCENECOMPONENT_H
#include "SCoreComponentBase.h"
#include "Maths/Vector.h"


class SSceneComponent : public SCoreComponentBase
{
public:

    FVector Location;
    FVector Scale;

};



#endif //SSCENECOMPONENT_H
