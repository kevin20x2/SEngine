//
// Created by kevin on 2024/10/13.
//

#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H
#include "SObject.h"


class SCameraComponent;

class SCameraManager : public SObject
{
    S_REGISTER_CLASS(SObject)

public:
    SCameraManager();
    virtual SCameraComponent * GetCamera() const
    {
        return Camera.get();
    }

protected:
    TSharedPtr<SCameraComponent> Camera;
};



#endif //CAMERAMANAGER_H
