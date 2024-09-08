//
// Created by kevin on 2024/8/17.
//

#include "SSceneComponent.h"
#include "glm/ext.hpp"

FVector SSceneComponent::GetForward() const
{
    auto Dir = FVector(1,0,0)* WorldTransform.Rotation;
    return glm::normalize(Dir);
}
