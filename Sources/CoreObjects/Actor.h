//
// Created by kevin on 2024/8/17.
//

#ifndef ACTOR_H
#define ACTOR_H
#include "SCoreComponentBase.h"
#include "SSceneComponent.h"
#include "Core/BaseTypes.h"


class SSceneComponent;

class SActor : public SObject
{
public:
	S_REGISTER_CLASS(SObject)

	using FComponentArray =  TArray <TSharedPtr<SCoreComponentBase>>;

	template <class T>
		T *  FindComponent()
	{
			for(auto Component : Components)
			{
				if(Component && Component->IsA<T>())
				{
					return Component.get();
				}
			}
	}

	void AddComponent(SCoreComponentBase * InComp )
	{
		auto CompPtr = InComp->AsShared();
		AddComponent(CompPtr);
	}

	void AddComponent(TSharedPtr <SCoreComponentBase> InComp)
	{
		if(!Components.Contains(InComp))
		{
			Components.Add(InComp);
		}
	}
	void SetRootComponent(SSceneComponent * InSceneComp);
	SSceneComponent * GetRootComponent() const
	{
		return RootComponent.get();
	}


protected:
    FComponentArray Components;

	TSharedPtr <SSceneComponent> RootComponent;
};



#endif //ACTOR_H
