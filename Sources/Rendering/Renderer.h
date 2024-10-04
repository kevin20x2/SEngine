//
// Created by kevin on 2024/8/11.
//

#ifndef RENDERER_H
#define RENDERER_H
#include "Core/BaseTypes.h"
#include "CoreObjects/CameraComponent.h"
#include "CoreObjects/PrimitiveComponent.h"
#include "GLFW/glfw3.h"
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
#include "RHI/ShaderProgram.h"
#include "RHI/Texture.h"
#include "Shader.h"


void OnRawWindowResize(GLFWwindow * Window, int Width, int Height);

class FRenderer
{
public:
    void Initailize();


    void Render();

    void OnResize(GLFWwindow * Window,int32 Width, int32 Height);

	FCommandBufferPool * GetCommandBufferPool() const;

	bool OnAddPrimitive(SPrimitiveComponent * InComp);
	bool OnRemovePrimitive(SPrimitiveComponent * InComp);
protected:

    TSharedPtr< SCameraComponent > Camera;

    //TSharedPtr<SPrimitiveComponent> Primitive;

	TArray < TSharedPtr<SPrimitiveComponent> > Primitives;

    TUniquePtr<FSwapChain> SwapChain;


    TUniquePtr<FDescriptorPool > DescriptorPool;

    TArray <
    TSharedPtr<FUniformBuffer > >  UniformBuffers;

	TSharedPtr <FTexture> Texture;

    TSharedPtr<FRenderPass> RenderPass;

	TArray <TSharedPtr <FDepthTexture> >  DepthTextures;

	TSharedPtr <FShader> Shader;

    TUniquePtr<FCommandBufferPool> CommandBufferPool;

    TUniquePtr<FCommandBuffers> CommandBuffers;

	void RecreateSwapChains();
    void RecreateFrameBuffers();

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
