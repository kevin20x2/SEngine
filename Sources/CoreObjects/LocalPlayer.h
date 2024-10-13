//
// Created by kevin on 2024/10/13.
//

#ifndef LOCALPLAYER_H
#define LOCALPLAYER_H
#include "SObject.h"


class SControllerBase;
class SCameraManager;

class SLocalPlayer : public SObject
{
    S_REGISTER_CLASS(SObject)

public:
    virtual SCameraManager * GetCameraManager();

    virtual SControllerBase * GetController();
protected:

    TSharedPtr<SCameraManager> CameraManager;
    TSharedPtr<SControllerBase  > Controller;

};


#endif //LOCALPLAYER_H
