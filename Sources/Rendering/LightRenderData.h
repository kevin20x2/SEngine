//
// Created by kevin on 2024/10/7.
//

#ifndef LIGHTRENDERDATA_H
#define LIGHTRENDERDATA_H
#include "Maths/Math.h"
#include "RHI/UniformBuffer.h"


struct FLightRenderDataShape
{
    FVector4 DirectionalLightDir = FVector4(1,1,1,1);
    FVector4 DirectionalLightColorAndOpacity = FVector4(1,1,1,1);
};


class FLightRenderData
{

public:
    FLightRenderData() ;

    FUniformBuffer * GetUniformBuffer(uint32 FrameIdx)
    {
        return UniformBuffers[FrameIdx].get();
    }
    void SyncData(uint32 CurrentFrame);


protected:

    void Initialize();

    FLightRenderDataShape RenderData;

    TArray <TSharedPtr <FUniformBuffer> > UniformBuffers;
};



#endif //LIGHTRENDERDATA_H
