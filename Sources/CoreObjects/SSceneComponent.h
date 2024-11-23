//
// Created by kevin on 2024/8/17.
//

#ifndef SSCENECOMPONENT_H
#define SSCENECOMPONENT_H
#include "Actor.h"
#include "SCoreComponentBase.h"
#include "Maths/Transform.h"

enum class ESceneComponentAttachmentRule
{
    KeepWorldTransform ,
    KeepRelativeTransform
};

class SSceneComponent : public SCoreComponentBase
{
public:
	S_REGISTER_CLASS(SCoreComponentBase)

    SSceneComponent( TSharedPtr <SActor > InOwner ) :
        SCoreComponentBase(InOwner) {}

    void SetWorldTransform(const FTransform & InWorld);

    FTransform GetWorldTransform() const
    {
        return WorldTransform;
    }
    void SetWorldLocation(const FVector & Location)
    {
        WorldTransform.Location = Location;
    }
    FVector GetWorldLocation() const
    {
        return WorldTransform.Location;
    }

    FQuat GetWorldRotation() const
    {
        return WorldTransform.Rotation;
    }

    void SetRotation(const FQuat & Rotation)
    {
        WorldTransform.Rotation = Rotation;
    }
    FVector GetForward() const ;
    FVector GetRight() const;
    FVector GetUp() const;

    SSceneComponent * GetParent() const
    {
        return Parent.get();
    }

    uint32 GetChildCount() const
    {
        return Children.size();
    }

    bool AttachToParent(SSceneComponent * InParent , ESceneComponentAttachmentRule AttachmentRule);

    bool DetachFromParent();

protected:

    void UpdateWorldTransform();

    bool AddChild(SSceneComponent * InChild);
    bool RemoveChild( SSceneComponent * InChild );

    TArray <TSharedPtr <SSceneComponent> > Children;

    TSharedPtr <SSceneComponent> Parent = nullptr;

    FTransform WorldTransform;

    FTransform RelativeTransform;
};



#endif //SSCENECOMPONENT_H
