//
// Created by kevin on 2024/8/18.
//

#include "Math.h"

/*
#include <cmath>

float FMath::Sin(float InValue)
{
    return sin(InValue);
}

float FMath::Cos(float InValue)
{
    return cos(InValue);
}
*/
FMatrix4 FTransform::ToMatrix() const
{
    auto TransMat=  glm::translate(FMatrix4(1.0f),Location);
    auto RotMat = glm::mat4_cast(Rotation);
    auto ScaleMat = glm::scale(FMatrix4(1.0f),Scale);

    return ScaleMat * RotMat * TransMat;
}
