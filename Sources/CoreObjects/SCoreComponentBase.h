//
// Created by kevin on 2024/8/17.
//

#ifndef SCORECOMPONENTBASE_H
#define SCORECOMPONENTBASE_H
#include "SObject.h"
#include "Core/BaseTypes.h"


class SActor;

class SCoreComponentBase : public SObject
{
public:
	S_REGISTER_CLASS(SObject)

    explicit SCoreComponentBase(const TSharedPtr<SActor> & InActor = nullptr) :
        Owner(InActor)
    {
    }

	SActor * GetOwner() const
	{
		return Owner.get();
	}
    virtual void OnRegister();
    virtual void OnUnRegister();

protected:
    TSharedPtr<SActor> Owner = nullptr;
};



#endif //SCORECOMPONENTBASE_H
