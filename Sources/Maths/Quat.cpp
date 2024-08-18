//
// Created by kevin on 2024/8/18.
//

#include "Quat.h"

#include "Math.h"


FQuat FQuat::Identity = FQuat();

FQuat::FQuat(float InW,float InX, float InY, float InZ ) :
 W(InW),X(InX),Y(InY),Z(InZ) { }

FQuat::FQuat(const FVector& Axis, float RotationInRad)
{
    float HalfAngle = RotationInRad * 0.5f;
    float SinHalfAngle = FMath::Sin(HalfAngle);
    W = FMath::Cos(HalfAngle);
    X = SinHalfAngle * Axis.X;
    Y = SinHalfAngle * Axis.Y;
    Z = SinHalfAngle * Axis.Z;
}

FQuat FQuat::Inverse() const
{
    return FQuat(W,-X,-Y,-Z);
}
