//
// Created by kevin on 2024/10/13.
//

#ifndef LOCALPLAYER_H
#define LOCALPLAYER_H
#include "PlayerController.h"
#include "SObject.h"


class SCameraManager;

class SLocalPlayer : public SObject
{
    S_REGISTER_CLASS(SObject)

public:
    SLocalPlayer();

    virtual SPlayerController * GetPlayerController()
    {
        return PlayerController.get();
    }
protected:
    TSharedPtr<SPlayerController  > PlayerController;

};


#endif //LOCALPLAYER_H
