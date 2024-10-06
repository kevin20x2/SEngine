//
// Created by kevin on 2024/8/11.
//

#ifndef RENDERER_H
#define RENDERER_H
#include "PrimitiveRenderData.h"
#include "SceneView.h"
#include "Core/BaseTypes.h"
#include "CoreObjects/CameraComponent.h"
#include "CoreObjects/PrimitiveComponent.h"
#include "GLFW/glfw3.h"
#include "RHI/CommandBuffer.h"
#include "RHI/DescriptorPool.h"
#include "RHI/SwapChain.h"
#include "RHI/UniformBuffer.h"
#include "RHI/FrameBuffer.h"
#include "RHI/RenderPass.h"
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

	FSceneView * GetSceneView()
	{
		return SceneView.get();
	}
	FPrimitiveRenderData * GetPrimitiveData()
	{
		return PrimitiveData.get();
	}

protected:

	TSharedPtr<SActor> Actor;

    TSharedPtr< SCameraComponent > Camera;


	TArray < TSharedPtr<SPrimitiveComponent> > Primitives;

    TUniquePtr<FSwapChain> SwapChain;


    TUniquePtr<FDescriptorPool > DescriptorPool;


	TSharedPtr<FSceneView > SceneView;

	TSharedPtr <FPrimitiveRenderData> PrimitiveData;

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
