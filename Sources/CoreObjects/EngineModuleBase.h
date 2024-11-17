//
// Created by vinkzheng on 2024/10/28.
//

#ifndef ENGINEMODULEBASE_H
#define ENGINEMODULEBASE_H
#include "Engine.h"
#include "SObject.h"
#include "Interfaces/TickableInterface.h"


class SEngineModuleBase : public SObject , public ITickable
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


    virtual void OnInitialize() {} ;
    virtual void OnPostInit() {};


protected:
};



#endif //ENGINEMODULEBASE_H
