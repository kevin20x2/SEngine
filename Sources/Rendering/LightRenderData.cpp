//
// Created by kevin on 2024/10/7.
//

#include "LightRenderData.h"

#include "RHI/RHI.h"

FLightRenderData::FLightRenderData()
{
    Initialize();
}

void FLightRenderData::SyncData(uint32 CurrentFrame)
{
    UniformBuffers[CurrentFrame]->UpdateData(&RenderData);
}

void FLightRenderData::Initialize()
{
    uint32 BufferCount = GRHI->GetMaxFrameInFlight();
    UniformBuffers.resize(BufferCount);

    for(auto & Buffer : UniformBuffers)
    {
        Buffer =
            TSharedPtr <FUniformBuffer> (
                FUniformBuffer::Create(sizeof(FLightRenderDataShape))
                );
        Buffer->UpdateData(&RenderData);
    }
}

