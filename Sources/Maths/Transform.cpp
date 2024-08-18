//
// Created by kevin on 2024/8/18.
//

#include "Transform.h"

FMatrix4 FTransform::ToMatrix() const
{
    FMatrix4 Result;

    Result.M4[3] = FVector4(Location,1.0f);

    return Result;
}
