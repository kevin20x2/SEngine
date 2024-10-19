//
// Created by vinkzheng on 2024/10/19.
//

#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H
#include "Controller.h"
#include "Input/Input.h"


class SCameraManager;
class SLocalPlayer;

class SPlayerController : public SControllerBase
{
    S_REGISTER_CLASS(SControllerBase)
public:
    SPlayerController();

    virtual void InitCameraInput();

    TSharedPtr<SCameraManager> CameraManager;

    TWeakPtr <SLocalPlayer > Player;
};



#endif //PLAYERCONTROLLER_H
