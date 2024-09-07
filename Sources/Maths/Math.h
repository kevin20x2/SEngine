//
// Created by kevin on 2024/8/18.
//

#ifndef MATH_H
#define MATH_H
//#include <Eigen/Dense>
#include <glm/glm.hpp>
#include <glm/ext.hpp>


using FQuat = glm::quat;
using FVector = glm::vec3;
using FVector4 = glm::vec4;

//using FTransform = Eigen::Transform<float,3,Eigen::Affine>;

using FMatrix4 = glm::mat4;

struct FTransform
{
    FVector Location = FVector(0,0,0);
    FQuat Rotation = FQuat(1,0,0,0);
    FVector Scale = FVector(1);

    FMatrix4 ToMatrix() const;
};

#endif //MATH_H
