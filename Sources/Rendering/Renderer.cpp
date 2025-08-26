//
// Created by kevin on 2024/8/11.
//

#include "Renderer.h"

#include <memory>
#include <spdlog/spdlog.h>

#include "volk.h"
#include "CoreObjects/CameraManager.h"
#include "CoreObjects/Engine.h"
#include "CoreObjects/LocalPlayer.h"
#include "GUI/SImGUI.h"
#include "Platform/FbxMeshImporter.h"
#include "RHI/RHI.h"
#include "Platform/Path.h"
#include "Platform/CImgTextureLoader.h"
#include "RHI/RenderTargetGroup.h"
#include "Systems/ShaderManager/ShaderManager.h"
#include "RenderTargetCube.h"
#include "Components/BRDFLUTRenderComponent.h"
#include "Components/IrradianceCubeComponent.h"

void OnRawWindowResize(GLFWwindow* Window, int Width, int Height)
{
	auto Renderer = GEngine->GetRenderer();
	GRHI->SetDisplaySize(Width,Height);
	Renderer->OnResize(Window,Width,Height);
}

void SRenderer::OnPostInit()
{
    SEngineModuleBase::OnPostInit();

	FCImgTextureLoader Loader;
	auto Texture =  Loader.LoadTexture(FPath::GetApplicationDir() + "/Assets/Textures/cloth.png" );
	FTextureCubeData  CubeTextureData ;
    Loader.LoadSingleCubeTextureData(FPath::GetApplicationDir() + "/Assets/Textures/canyon1.png", CubeTextureData);

    auto CubeRT = FRenderTargetCube ::Create(CubeTextureData.Width,CubeTextureData.Height,CubeTextureData.InData.data());
	auto IrradianceCubeTex = FRenderTargetCube::Create(CubeTextureData.Width,CubeTextureData.Height,CubeTextureData.InData.data());
    ReflectionCapture = std::make_shared<SReflectionCaptureComponent>();
    ReflectionCapture->SetCubeRT(CubeRT);
    ReflectionCapture->Init();

	IrradianceCube = std::make_shared<SIrradianceCubeComponent>();
	IrradianceCube->SetCubeRT(IrradianceCubeTex);
	IrradianceCube->Init();

	BRDFLUTRenderer = std::make_shared<SBRDFLUTRenderComponent>();
	//BRDFLUTRenderer->Init();

	auto Shader = SShaderManager::GetShaderFromName("test");
	auto Material = TSharedPtr<SMaterialInterface>(new SMaterialInterface( Shader->AsShared() ) );
	Material->Initialize(DescriptorPool->Pool,GetRenderPass());
	Material->SetTexture(3,Texture);
	FFbxMeshImporter Importer;
	Actor = Importer.LoadAsSingleActor(FPath::GetApplicationDir() +  "/Assets/Sphere.fbx",Material.get());


    Material->SetTextureCube(5,CubeRT->GetCubeTexture());
	Material->SetSampler("samplerCube",CubeRT->GetCubeTexture());
	Material->SetTextureCube("IrradianceCube",IrradianceCubeTex->GetCubeTexture());

    ReflectionCapture->FilterCubeMap();
	IrradianceCube->GenerateIrradianceCubeMap();
}

void SRenderer::OnInitialize()
{

    uint32 MaxFrameInFlight = GRHI->GetMaxFrameInFlight();

	CommandBufferPool = TUniquePtr<FCommandBufferPool>(new FCommandBufferPool());

    DescriptorPool = TUniquePtr<FDescriptorPool>(
        FUniformBufferDescriptorPool::Create(100 * MaxFrameInFlight,200 * MaxFrameInFlight)
        );

	RecreateSwapChains();

    CommandBuffers = TUniquePtr<FCommandBuffers>(new FCommandBuffers(MaxFrameInFlight,CommandBufferPool.get()));


    SceneView = TSharedPtr<FSceneView>(new FSceneView);

    PrimitiveData = TSharedPtr<FPrimitiveRenderData>(new FPrimitiveRenderData());

    LightData = SNew<FLightRenderData>();

	ShadowRTG = TSharedPtr<FRenderTargetGroup>(new FRenderTargetGroup());
	BaseRTG = TSharedPtr<FRenderTargetGroup>(new FRenderTargetGroup());

	FRenderTargetGroupCreateParams CreateParams =
		{
		.Width = SwapChain->GetExtent().width,
		.Height = SwapChain->GetExtent().height
		};

	ShadowRTG->Initialize( CreateParams );

	CreateParams.RenderTexNum = 2;
	BaseRTG->Initialize(CreateParams);

	SwapChainRTG = TSharedPtr <FRenderTargetGroup>(new FRenderTargetGroup());
	SwapChainRTG->InitializeBySwapChain(SwapChain.get());

	PostProcessManager.reset(new FPostProcessManager);
	PostProcessManager->InitRenderResource();

	CreateSyncObjects();


}

void SRenderer::Render()
{


	auto Device = *GRHI->GetDevice();
	vkWaitForFences(Device,1, &InFlightFences[CurrentFrame], VK_TRUE , UINT64_MAX);

    FUniformGlobalStagingBuffer::GetInstance()->ResetOffset();

	uint32_t ImageIndex ;
	VkResult Result = vkAcquireNextImageKHR(
		Device,*SwapChain->GetSwapChain(),UINT64_MAX,ImageAvailableSems[CurrentFrame],
		VK_NULL_HANDLE,&ImageIndex );

	vkResetFences(Device,1,&InFlightFences[CurrentFrame]);


	auto Camera = GEngine->GetLocalPlayer()->GetPlayerController()->CameraManager->GetCamera();

	//IrradianceCube->GenerateIrradianceCubeMap();

    auto CommandBuffer = CommandBuffers->Buffers[CurrentFrame];

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;
    VK_CHECK(vkBeginCommandBuffer(CommandBuffer, &beginInfo));


	SceneView->UpdateViewData(Camera);
	SceneView->SyncData(CommandBuffer,CurrentFrame);
	LightData->SyncData(CommandBuffer,CurrentFrame);
	//GEngine->GetGUIPort()->OnRecordGUIData();


    PreRecordCommandBuffer(CommandBuffer,ImageIndex);
	RecordCommandBuffer(CommandBuffer,ImageIndex);

    VK_CHECK(vkEndCommandBuffer(CommandBuffer));

	// Submit
	VkSubmitInfo submitInfo {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = {ImageAvailableSems[CurrentFrame]};
	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;

	submitInfo.pCommandBuffers = &CommandBuffers->Buffers[CurrentFrame];

	VkSemaphore SignalSemaphores[] = { RenderFinishSems[CurrentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = SignalSemaphores;

	VK_CHECK(vkQueueSubmit(
		*GRHI->GetGraphicsQueue()
		,1,&submitInfo,InFlightFences[CurrentFrame]));

	// Present
	VkPresentInfoKHR PresentInfo = {};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = SignalSemaphores;

	VkSwapchainKHR swapChains [] = {*SwapChain->GetSwapChain()};

	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = swapChains;
	PresentInfo.pImageIndices = &ImageIndex;
	PresentInfo.pResults = nullptr;

	Result = vkQueuePresentKHR(
		*GRHI->GetPresentQueue() ,&PresentInfo);

	CurrentFrame = (CurrentFrame + 1) % GRHI->GetMaxFrameInFlight();
}

void SRenderer::OnResize(GLFWwindow* Window, int32 Width, int32 Height)
{
	printf("Resize %d %d",Width,Height);
    SwapChainRTG->CleanUp();

	RecreateSwapChains();

	SwapChainRTG->InitializeBySwapChain(SwapChain.get());

	for(auto Primitive  : Primitives)
	{
		if(Primitive)
		{
			Primitive->GetMaterial()->CreatePipeline(GetRenderPass());
		}
	}
	FRenderTargetGroupCreateParams Params = {
		.Width= (uint32)Width,
		.Height = (uint32)Height
	};
	ShadowRTG->Initialize(Params);
	Params.RenderTexNum = 2;
	BaseRTG->Initialize(Params);
}


void SRenderer::CreateSyncObjects()
{
    int32 Count = GRHI->GetMaxFrameInFlight();
	ImageAvailableSems.resize(Count);
	RenderFinishSems.resize(Count);
	InFlightFences.resize(Count);

	VkSemaphoreCreateInfo SemaphoreInfo {};
	SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo FenceInfo {};
	FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    auto Device = *GRHI->GetDevice();
	for(int32 i = 0 ;i < Count; ++ i)
	{
		VK_CHECK(vkCreateSemaphore(Device, &SemaphoreInfo,nullptr, &ImageAvailableSems[i]));

		VK_CHECK(vkCreateSemaphore(Device, &SemaphoreInfo,nullptr, &RenderFinishSems[i]));

		VK_CHECK(vkCreateFence(Device,&FenceInfo,nullptr,&InFlightFences[i]));
	}
}

void SRenderer::PreRecordCommandBuffer(VkCommandBuffer CommandBuffer,uint32 ImageIndex)
{
	FPreRecordBufferContext Context = {
		.RenderPass = GetRenderPass()
	};
	for(auto P : Primitives)
	{
		if(P)
		{
			P->OnPreRecordCommandBuffer(CommandBuffer,CurrentFrame,Context);
		}
	}

	PostProcessManager->OnPreRecordCommandBuffer(CommandBuffer,ImageIndex,BaseRTG.get(),SwapChainRTG.get());
}

void SRenderer::RecordCommandBuffer(VkCommandBuffer CommandBuffer, uint32 ImageIndex)
{


	BaseRTG->BeginRenderTargetGroup(CommandBuffer,ImageIndex, FVector4(0,0,0,1.0f));

	for(auto P : Primitives)
	{
		if(P)
		{
			P->OnRecordCommandBuffer(CommandBuffer,CurrentFrame);
		}
	}

	BaseRTG->EndRenderTargetGroup(CommandBuffer);

	SwapChainRTG->BeginRenderTargetGroup(CommandBuffer,ImageIndex,FVector4(0,0,0,1));

	PostProcessManager->BeginPostProcess(CurrentFrame,CommandBuffer,BaseRTG.get(),SwapChainRTG.get());
	//GEngine->GetGUIPort()->OnRender(CommandBuffer);
	SwapChainRTG->EndRenderTargetGroup(CommandBuffer);

}
FCommandBufferPool *
SRenderer::GetCommandBufferPool() const
{
	return CommandBufferPool.get();
}

void SRenderer::RecreateSwapChains()
{

	if(SwapChain)
	{
		SwapChain->CleanUp();
	}
	SwapChain = std::unique_ptr<FSwapChain>(
		FSwapChain::CreateSwapChain(GRHI->GetDisplaySize()) );

}
bool SRenderer::OnAddPrimitive(SPrimitiveComponent *InComp)
{
	if(InComp == nullptr) return false;

	if(Primitives.Contains(InComp->AsShared()))
	{
		spdlog::warn("Primitive already contained");
		return false;
	}
	Primitives.Add(InComp->AsShared());
	return true;
}
bool SRenderer::OnRemovePrimitive(SPrimitiveComponent *InComp)
{
	if(InComp == nullptr) return false;

	if(Primitives.Contains(InComp->AsShared()))
	{
		Primitives.Remove(InComp->AsShared());
		return true;
	}
	return false;
}
