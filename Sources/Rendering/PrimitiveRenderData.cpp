//
// Created by kevin on 2024/10/6.
//

#include "PrimitiveRenderData.h"

#include "RHI/RHI.h"

FPrimitiveRenderData::FPrimitiveRenderData()
{
    Initialize();
}

void FPrimitiveRenderData::UpdateModelMatrix(const FMatrix4& InMat)
{
    RenderData.ModelMatrix = InMat;
}

void FPrimitiveRenderData::SyncData(uint32 CurrentFrame)
{
    UniformBuffers[CurrentFrame]->UpdateData(&RenderData);
}

void FPrimitiveRenderData::Initialize()
{
    uint32 BufferCount = GRHI->GetMaxFrameInFlight();
    UniformBuffers.resize(BufferCount);
    for(auto & Buffer : UniformBuffers)
    {
        Buffer =
            TSharedPtr<FUniformBuffer>(
            FUniformBuffer::Create(sizeof(FPerPrimitiveRenderDataShape)));
    }
}
