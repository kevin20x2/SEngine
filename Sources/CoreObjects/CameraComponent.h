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
	S_REGISTER_CLASS(SSceneComponent)

    SCameraComponent() {}

    FMatrix4 GetViewMatrix();
    FMatrix4 GetProjectinMatrix();

    float Fov = 0.333f* glm::pi<float>();
    float NearPlane = 0.1;
    float FarPlane = 10000;
};


#endif //CAMERACOMPONENT_H
