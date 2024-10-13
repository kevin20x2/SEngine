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
    virtual SCameraComponent * GetCamera();
protected:
    TSharedPtr<SCameraComponent> CurrentCamera;

    TArray < TSharedPtr<SCameraComponent> > CameraList;
};



#endif //CAMERAMANAGER_H
