//
// Created by kevin on 2024/8/11.
//

#ifndef RENDERER_H
#define RENDERER_H
#include "LightRenderData.h"
#include "PostProcessManager.h"
#include "PrimitiveRenderData.h"
#include "SceneView.h"
#include "Core/BaseTypes.h"
#include "CoreObjects/PrimitiveComponent.h"
#include "GLFW/glfw3.h"
#include "RHI/CommandBuffer.h"
#include "RHI/DescriptorPool.h"
#include "RHI/SwapChain.h"
#include "RHI/FrameBuffer.h"
#include "RHI/RenderPass.h"
#include "RHI/Texture2D.h"
#include "Shader.h"
#include "CoreObjects/EngineModuleBase.h"
#include "RHI/RenderTargetGroup.h"


class FRenderTargetGroup;
void OnRawWindowResize(GLFWwindow * Window, int Width, int Height);

struct FPreRecordBufferContext
{
	FRenderPass * RenderPass;

};

class SRenderer : public SEngineModuleBase
{
public:
	S_REGISTER_CLASS(SEngineModuleBase)

	virtual void OnPostInit() override;
	virtual void OnInitialize() override;

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

	FLightRenderData * GetLightData()
	{
		return LightData.get();
	}
	FDescriptorPool * GetDescriptorPool()
	{
		return DescriptorPool.get();
	}

	FRenderPass * GetRenderPass()
	{
		return BaseRTG->GetRenderPass();
	}

	FRenderPass * GetSwapChainRenderPass()
	{
		return SwapChainRTG->GetRenderPass();
	}
	FRenderTargetGroup * GetBaseRenderTargetGroup()
	{
		return BaseRTG.get();
	}

	uint32 GetFrameIndex() const
	{
		return CurrentFrame;
	}

protected:

	TSharedPtr<SActor> Actor;

	TArray < TSharedPtr<SPrimitiveComponent> > Primitives;

    TUniquePtr<FSwapChain> SwapChain;

    TUniquePtr<FDescriptorPool > DescriptorPool;

	TSharedPtr<FSceneView > SceneView;

	TSharedPtr <FPrimitiveRenderData> PrimitiveData;

	TSharedPtr <FLightRenderData> LightData;


    TUniquePtr<FCommandBufferPool> CommandBufferPool;

    TUniquePtr<FCommandBuffers> CommandBuffers;

	void RecreateSwapChains();

	TSharedPtr <FRenderTargetGroup> SwapChainRTG;
	TSharedPtr <FRenderTargetGroup>  ShadowRTG;
	TSharedPtr <FRenderTargetGroup> BaseRTG;

	TSharedPtr <FPostProcessManager> PostProcessManager;

    void CreateSyncObjects();

	void PreRecordCommandBuffer(uint32 ImageIndex);
    void RecordCommandBuffer(VkCommandBuffer CommandBuffer , uint32 ImageIndex);

    TArray <VkSemaphore> ImageAvailableSems;
    TArray <VkSemaphore> RenderFinishSems;
    TArray <VkFence> InFlightFences;

    uint32 CurrentFrame = 0;
};


#endif //RENDERER_H
