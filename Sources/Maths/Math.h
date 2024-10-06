//
// Created by kevin on 2024/8/18.
//

#ifndef MATH_H
#define MATH_H
//#include <Eigen/Dense>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Core/BaseTypes.h"


using FQuat = glm::quat;
using FVector = glm::vec3;
using FVector4 = glm::vec4;
using FVector2 = glm::vec2;

//using FTransform = Eigen::Transform<float,3,Eigen::Affine>;

FString ToString(const FVector & Vec);

using FMatrix4 = glm::mat4;

struct FTransform
{
    FVector Location = FVector(0,0,0);
    FQuat Rotation = FQuat(1,0,0,0);
    FVector Scale = FVector(1);

    static void Multiply(FTransform & Out , const FTransform &  Lhs, const FTransform & Rhs );

    FTransform operator*(const FTransform & Rhs)
    {
        FTransform Out;
        Multiply(Out,*this,Rhs);
        return Out;
    }

    FTransform & Invert();

    FMatrix4 ToMatrix() const;

    void FromMatrix(const FMatrix4 & InMat);

};

#endif //MATH_H
