//
// Created by kevin on 2024/8/17.
//

#include "SSceneComponent.h"

#include <spdlog/spdlog.h>

#include "glm/ext.hpp"

void SSceneComponent::SetWorldTransform(const FTransform& InWorld)
{
    WorldTransform = InWorld;
    if(Parent)
    {
        auto InvParentWorld = Parent->GetWorldTransform().Invert();
        auto NewRelative = InvParentWorld *  InWorld ;
        RelativeTransform = NewRelative;
    }
}

FVector SSceneComponent::GetForward() const
{
    auto Dir =  WorldTransform.Rotation *FVector(1,0,0);
    return glm::normalize(Dir);
}

bool SSceneComponent::AttachToParent(SSceneComponent* InParent,
    ESceneComponentAttachmentRule AttachmentRule)
{
    if(!InParent) return false;
    if(Parent.get() == InParent)
    {
        spdlog::warn("already attached to parent");
        return false;
    }
    if(Parent != nullptr)
    {
        DetachFromParent();
    }

    Parent = InParent->AsShared();
    InParent->AddChild(this);

    if(AttachmentRule == ESceneComponentAttachmentRule::KeepRelativeTransform)
    {
        UpdateWorldTransform();
    }
    else
    {
        SetWorldTransform(WorldTransform);
        //auto InvParentWorld = Parent->GetWorldTransform().Invert();
        //auto NewRelative = InvParentWorld *  GetWorldTransform() ;
        //RelativeTransform = NewRelative;
    }
    return true;
}

bool SSceneComponent::DetachFromParent()
{
    if(Parent)
    {
        bool Result = Parent->RemoveChild(this);
        Parent = nullptr;
        return Result;
    }
    return false;
}

void SSceneComponent::UpdateWorldTransform()
{
    if(Parent)
    {
        WorldTransform = Parent->GetWorldTransform() * RelativeTransform;
    }
}

bool SSceneComponent::RemoveChild(SSceneComponent* InChild)
{
    if(!InChild) return false;
    if(Children.Contains(InChild->AsShared()))
    {
        return Children.Remove(InChild->AsShared());
    }
    return false;
}

bool SSceneComponent::AddChild(SSceneComponent* InChild)
{
    return Children.AddUnique(InChild->AsShared());
}
