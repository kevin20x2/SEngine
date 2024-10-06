//
// Created by kevin on 2024/10/6.
//

#ifndef PRIMITIVERENDERDATA_H
#define PRIMITIVERENDERDATA_H
#include "Maths/Math.h"
#include "RHI/UniformBuffer.h"


struct FPerPrimitiveRenderDataShape
{
    FMatrix4 ModelMatrix;
};

class FPrimitiveRenderData
{
public:
    FPrimitiveRenderData();
    void UpdateModelMatrix(const FMatrix4 & InMat);
    void SyncData(uint32 CurrentFrame);

    FUniformBuffer * GetUniformBuffer(uint32 FrameIdx)
    {
        return UniformBuffers[FrameIdx].get();
    }

protected:
    void Initialize();
    FPerPrimitiveRenderDataShape RenderData;

    TArray <TSharedPtr<FUniformBuffer> > UniformBuffers;
};



#endif //PRIMITIVERENDERDATA_H
