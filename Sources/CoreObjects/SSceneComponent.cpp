//
// Created by kevin on 2024/8/17.
//

#include "SSceneComponent.h"

FVector SSceneComponent::GetWorldLocation() const
{
    return WorldTransform.translation();
}
