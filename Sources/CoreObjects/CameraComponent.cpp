//
// Created by kevin on 2024/8/17.
//

#include "CameraComponent.h"
#include <glm/ext/matrix_clip_space.hpp>


FMatrix4 SCameraComponent::GetViewMatrix()
{
    auto WorldTrans =  GetWorldTransform().ToMatrix();
    auto InvWorld =  inverse(WorldTrans);
    auto SwapMat  = FMatrix4({0,0,1,0}, {1,0,0,0}, {0,-1,0,0},{0,0,0,1});

    return glm::transpose(SwapMat* InvWorld );
}

FMatrix4 SCameraComponent::GetProjectinMatrix()
{
    auto TransP = glm::perspective(Fov,4.0f/3.0f,NearPlane,FarPlane);
    return glm::transpose(TransP);
}
