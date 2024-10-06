//
// Created by kevin on 2024/10/6.
//

#ifndef SCENEVIEW_H
#define SCENEVIEW_H
#include "Maths/Math.h"
#include "RHI/UniformBuffer.h"


class SCameraComponent;

struct FViewDataShape
{
    FMatrix4 ViewProjectionMat;
    FVector ViewOrigin;
};

class FSceneView
{

public:
    FSceneView();

    FUniformBuffer * GetUniformBuffer(uint32 Idx)
    {
        return UniformBuffers[Idx].get();
    }


    void UpdateViewData(SCameraComponent * InCameraComp);
    void SyncData(uint32 CurrentFrame);
protected:
    void Initialize();

    FViewDataShape ViewData;
    TArray< TSharedPtr<FUniformBuffer> > UniformBuffers;
};



#endif //SCENEVIEW_H
