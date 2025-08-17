//
// Created by kevin on 2024/10/7.
//

#include "LightRenderData.h"

#include "RHI/RHI.h"

FLightRenderData::FLightRenderData()
{
    Initialize();
}

void FLightRenderData::SyncData(VkCommandBuffer CommandBuffer,uint32 CurrentFrame)
{
    UniformBuffers[CurrentFrame]->UpdateData(CommandBuffer,&RenderData);
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
    }
}

