//
// Created by kevin on 2024/8/17.
//

#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H
#include "SCoreComponentBase.h"
#include "SSceneComponent.h"
#include "glm/glm.hpp"


class SCameraComponent : public SSceneComponent
{
public:
    SCameraComponent() {}

    FMatrix4 GetViewMatrix();
    FMatrix4 GetProjectinMatrix();

    float Fov = 0.333f* glm::pi<float>();
    float NearPlane = 1;
    float FarPlane = 100;
};


#endif //CAMERACOMPONENT_H
