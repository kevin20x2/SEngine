//
// Created by kevin on 2024/8/17.
//

#ifndef ACTOR_H
#define ACTOR_H
#include "SCoreComponentBase.h"
#include "Core/BaseTypes.h"


class SActor
{
public:
    using FComponentArray =  TArray <TSharedPtr<SCoreComponentBase>>;


    FComponentArray Components;

};



#endif //ACTOR_H
