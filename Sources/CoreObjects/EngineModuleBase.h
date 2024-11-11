//
// Created by vinkzheng on 2024/10/28.
//

#ifndef ENGINEMODULEBASE_H
#define ENGINEMODULEBASE_H
#include "Engine.h"
#include "SObject.h"


class SEngineModuleBase : public SObject
{
    S_REGISTER_CLASS( SObject);
public:

    template <typename SubType>
    static SubType * GetEngineModule()
    {
        if(GEngine)
        {
            return GEngine->GetModuleByClass<SubType>();
        }
        return nullptr;
    }

    virtual void Tick(float DeltaTime) {} ;

    bool IsTickable()const
    {
        return bTickable;
    }
protected:
    bool bTickable = false;
};



#endif //ENGINEMODULEBASE_H
