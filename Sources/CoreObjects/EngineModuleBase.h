//
// Created by vinkzheng on 2024/10/28.
//

#ifndef ENGINEMODULEBASE_H
#define ENGINEMODULEBASE_H
#include "SObject.h"


class SEngineModuleBase : public SObject
{
    S_REGISTER_CLASS(SEngineModuleBase, SObject);
public:

    virtual void Tick(float DeltaTime) {} ;

    bool IsTickable()const
    {
        return bTickable;
    }
protected:
    bool bTickable = false;
};



#endif //ENGINEMODULEBASE_H
