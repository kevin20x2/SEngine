//
// Created by kevin on 2024/8/17.
//

#include "Actor.h"

void SActor::SetRootComponent(SSceneComponent* InSceneComp)
{
	if (!InSceneComp) return;
	RootComponent = InSceneComp->AsShared();
}
