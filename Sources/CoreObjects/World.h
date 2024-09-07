//
// Created by kevin on 2024/9/3.
//

#ifndef WORLD_H
#define WORLD_H
#include "SObject.h"
#include "Core/BaseTypes.h"


class SActor;

class SWorld : public SObject
{

    void Tick(float DeltaTime);

    using FActorArray = TArray <TSharedPtr<SActor>>;
    FActorArray Actors;

};



#endif //WORLD_H
