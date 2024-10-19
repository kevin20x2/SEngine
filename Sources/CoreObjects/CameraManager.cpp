//
// Created by kevin on 2024/10/13.
//

#include "CameraManager.h"
#include "CameraComponent.h"

SCameraManager::SCameraManager()
{
    Camera = SNew<SCameraComponent>(nullptr);
}
