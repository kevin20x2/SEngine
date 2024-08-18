//
// Created by kevin on 2024/8/18.
//

#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "Matrix4.h"
#include "Quat.h"
#include "Vector.h"


struct FTransform
{
    FTransform() :
    Location(FVector::ZeroVector),
    Rotation(FQuat::Identity),
    Scale(FVector::OneVector)
    {}


    FMatrix4 ToMatrix() const;

    FVector Location;
    FQuat Rotation;
    FVector Scale;
};



#endif //TRANSFORM_H
