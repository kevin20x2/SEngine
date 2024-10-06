//
// Created by kevin on 2024/8/18.
//

#include "Math.h"

FString ToString(const FVector& Vec)
{
    char MaxBuffer[256];
    sprintf(MaxBuffer,"%f %f %f", Vec.x,Vec.y,Vec.z);
    return MaxBuffer;
}

void FTransform::Multiply(FTransform& Out, const FTransform& Lhs, const FTransform& Rhs)
{

    Out.Location = Rhs.Rotation *( Rhs.Scale  * Lhs.Location ) + Rhs.Location;
    Out.Rotation = Lhs.Rotation * Rhs.Rotation;
    Out.Scale = Lhs.Scale * Rhs.Scale;
}

FTransform & FTransform::Invert()
{
    Location = - Location;
    Rotation = inverse(Rotation);
    Scale = 1.0f / (Scale);
    return *this;
}

FMatrix4 FTransform::ToMatrix() const
{
    auto TransMat=  glm::translate(FMatrix4(1.0f),Location);
    auto RotMat = glm::mat4_cast(Rotation);
    auto ScaleMat = glm::scale(FMatrix4(1.0f),Scale);

    return TransMat * RotMat * ScaleMat;
}

void FTransform::FromMatrix(const FMatrix4& InMat)
{
}
