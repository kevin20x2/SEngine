//
// Created by kevin on 2024/8/11.
//

#ifndef RENDERER_H
#define RENDERER_H
#include "Core/BaseTypes.h"
#include "CoreObjects/CameraComponent.h"
#include "RHI/CommandBuffer.h"
#include "RHI/DescriptorPool.h"
#include "RHI/DescriptorSetLayout.h"
#include "RHI/SwapChain.h"
#include "RHI/UniformBuffer.h"
#include "RHI/DescriptorSet.h"
#include "RHI/FrameBuffer.h"
#include "RHI/GraphicsPipeline.h"
#include "RHI/IndexBuffer.h"
#include "RHI/RenderPass.h"
#include "RHI/Shader.h"


class FRenderer
{
public:
    void Initailize();


    void Render();

protected:

    SCameraComponent * Camera;

    TUniquePtr<FSwapChain> SwapChain;

    TUniquePtr<FDescriptorSetLayout> DescriptorSetLayout;

    TUniquePtr<FDescriptorPool > DescriptorPool;

    TUniquePtr<FDescriptorSets> DescriptorSets;

    TArray <
    TSharedPtr<FUniformBuffer > >  UniformBuffers;

    TSharedPtr<FRenderPass> RenderPass;

    TSharedPtr<FVertexShaderProgram> VertexShader;
    TSharedPtr<FPixelShaderProgram> PixelShader;

    TSharedPtr <FVertexBuffer> BaseVertexBuffer;
    TSharedPtr <FIndexBuffer> IndexBuffer;

    TUniquePtr<FGraphicsPipeline> Pipeline;

    TUniquePtr<FCommandBufferPool> CommandBufferPool;

    TUniquePtr<FCommandBuffers> CommandBuffers;

    TArray <
        TSharedPtr <FFrameBuffer >
        > FrameBuffers;

    void CreateSyncObjects();

    void RecordCommandBuffer(VkCommandBuffer CommandBuffer , uint32 ImageIndex);

    TArray <VkSemaphore> ImageAvailableSems;
    TArray <VkSemaphore> RenderFinishSems;
    TArray <VkFence> InFlightFences;

    uint32 CurrentFrame = 0;
};


#endif //RENDERER_H
