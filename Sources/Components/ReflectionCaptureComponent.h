//
// Created by kevin on 2025/8/17.
//

#ifndef SENGINE_REFLECTIONCAPTURECOMPONENT_H
#define SENGINE_REFLECTIONCAPTURECOMPONENT_H


#include "CoreObjects/SCoreComponentBase.h"
#include "Rendering/Material.h"

class FRenderTargetCube;

class SReflectionCaptureComponent : public SCoreComponentBase
{
    S_REGISTER_CLASS(SReflectionCaptureComponent)
public:

    void Init();

    void SetCubeRT(TSharedPtr <FRenderTargetCube> InCubeRT)
    {
        CubeRT = InCubeRT;
    }

    void FilterCubeMap(VkCommandBuffer CommandBuffer);

protected:

    TSharedPtr <SMaterialInterface> CubeFilterMaterial;

    TSharedPtr <FRenderTargetCube> CubeRT;
};


#endif //SENGINE_REFLECTIONCAPTURECOMPONENT_H
