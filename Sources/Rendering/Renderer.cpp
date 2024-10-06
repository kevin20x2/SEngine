//
// Created by kevin on 2024/8/11.
//

#include "Renderer.h"

#include <memory>
#include <spdlog/spdlog.h>

#include "volk.h"
#include "CoreObjects/Engine.h"
#include "Maths/Math.h"
#include "Platform/FbxMeshImporter.h"
#include "RHI/RHI.h"
#include "Platform/Path.h"
#include "Platform/CImgTextureLoader.h"
#include "RHI/DepthTexture.h"

void OnRawWindowResize(GLFWwindow* Window, int Width, int Height)
{
	auto Renderer = GEngine->GetRenderer();
	GRHI->SetDisplaySize(Width,Height);
	Renderer->OnResize(Window,Width,Height);
}

void FRenderer::Initailize()
{

	auto VertexShader = std::make_shared<FVertexShaderProgram>(FPath::GetApplicationDir()+  "/shaders/test.vert");
	auto PixelShader = std::make_shared<FPixelShaderProgram>(FPath::GetApplicationDir() + "/shaders/test.frag");

	Shader = TSharedPtr<FShader>(new FShader(VertexShader,PixelShader));


	SceneView = TSharedPtr<FSceneView>(new FSceneView);

	PrimitiveData = TSharedPtr<FPrimitiveRenderData>(new FPrimitiveRenderData());

    uint32 MaxFrameInFlight = GRHI->GetMaxFrameInFlight();

	CommandBufferPool = TUniquePtr<FCommandBufferPool>(new FCommandBufferPool());

	FCImgTextureLoader Loader;
	Texture =  Loader.LoadTexture(FPath::GetApplicationDir() + "/Assets/Textures/phptze1bz.png" );

    DescriptorPool = TUniquePtr<FDescriptorPool>(
        FUniformBufferDescriptorPool::Create(MaxFrameInFlight,MaxFrameInFlight)
        );

	RecreateSwapChains();


    RenderPass = TSharedPtr<FRenderPass>(
        FRenderPass::Create(SwapChain.get())
        );

	auto Material = TSharedPtr<SMaterialInterface>(new SMaterialInterface(
		Shader
		) );
	Material->Initialize(DescriptorPool->Pool,RenderPass.get());
	//Material->SetupViewData(UniformBuffers);
	Material->SetTexture(2,Texture);


    CommandBuffers = TUniquePtr<FCommandBuffers>(new FCommandBuffers(MaxFrameInFlight,CommandBufferPool.get()));

	RecreateFrameBuffers();

	auto Primitive = TSharedPtr<SPrimitiveComponent>(new SPrimitiveComponent(nullptr));
	Primitive->SetMaterial(Material);
	TSharedPtr< FStaticMesh> StaticMesh = TSharedPtr<FStaticMesh>(new FStaticMesh({},{}));
	Primitive->SetStaticMesh(StaticMesh);
	FFbxMeshImporter Importer;
	Importer.ImportMesh(FPath::GetApplicationDir() +  "/Assets/gy.fbx",StaticMesh.get());

	Primitive->CreateRHIResource();
	Primitive->OnRegister();
	CreateSyncObjects();
	Camera = TSharedPtr<SCameraComponent>( new SCameraComponent(nullptr));
	Camera->SetWorldLocation({-3,0,1});

	GEngine->GetInput()->BindKey(GLFW_KEY_W, [WeakCamera = TWeakPtr<SCameraComponent>(Camera)]()
	{
		auto SharedCamera=WeakCamera.lock();
		if(SharedCamera != nullptr)
		{
			auto OldPos = SharedCamera->GetWorldLocation();
			OldPos.z += 0.1f;
			SharedCamera->SetWorldLocation(OldPos);
		}
	});

	GEngine->GetInput()->BindKey(GLFW_KEY_S, [WeakCamera = TWeakPtr<SCameraComponent>(Camera)]()
	{
		auto SharedCamera=WeakCamera.lock();
		if(SharedCamera != nullptr)
		{
			auto OldPos = SharedCamera->GetWorldLocation();
			OldPos.z -= 0.1f;
			SharedCamera->SetWorldLocation(OldPos);
		}
	});

	GEngine->GetInput()->BindScrollOperation([WeakCamera = TWeakPtr<SCameraComponent>(Camera)](double Value)
	{
		auto SharedCamera=WeakCamera.lock();
		if(SharedCamera != nullptr)
		{
			auto Forward = SharedCamera->GetForward();
			spdlog::info("dir: {}",ToString(Forward));
			float Scale = 0.1f;
			auto OldPos = SharedCamera->GetWorldLocation();
			OldPos += Scale *(float)(Value)* Forward;
			SharedCamera->SetWorldLocation(OldPos);
		}
	});


}

void FRenderer::Render()
{

	auto Device = *GRHI->GetDevice();
	vkWaitForFences(Device,1, &InFlightFences[CurrentFrame], VK_TRUE , UINT64_MAX);

	uint32_t ImageIndex ;
	VkResult Result = vkAcquireNextImageKHR(
		Device,*SwapChain->GetSwapChain(),UINT64_MAX,ImageAvailableSems[CurrentFrame],
		VK_NULL_HANDLE,&ImageIndex );

	vkResetFences(Device,1,&InFlightFences[CurrentFrame]);

	SceneView->UpdateViewData(Camera.get());
	SceneView->SyncData(CurrentFrame);

	RecordCommandBuffer(CommandBuffers->Buffers[CurrentFrame],ImageIndex);

	// Submit
	VkSubmitInfo submitInfo {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = {ImageAvailableSems[CurrentFrame]};
	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
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

void FRenderer::OnResize(GLFWwindow* Window, int32 Width, int32 Height)
{
	printf("Resize %d %d",Width,Height);
	RecreateSwapChains();
    RenderPass = TSharedPtr<FRenderPass>(
        FRenderPass::Create(SwapChain.get())
        );
	RecreateFrameBuffers();
	for(auto Primitive  : Primitives)
	{
		if(Primitive)
		{
			Primitive->GetMaterial()->CreatePipeline(RenderPass.get());
		}
	}
	//RecreatePipeline();
	CommandBuffers->FreeCommandBuffer();
}



void FRenderer::RecreateFrameBuffers()
{
	FrameBuffers.clear();
    uint32 MaxFrameInFlight = GRHI->GetMaxFrameInFlight();
    FrameBuffers.resize(MaxFrameInFlight);
    int32 ViewIdx = 0;
    for(auto & FrameBuffer : FrameBuffers )
    {
        FrameBuffer = TSharedPtr <FFrameBuffer>( new FFrameBuffer(
            ViewIdx,RenderPass.get(),SwapChain.get(),DepthTextures
            ));
        ViewIdx ++ ;
    }
}

void FRenderer::CreateSyncObjects()
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
	for(size_t i = 0 ;i < Count; ++ i)
	{
		VK_CHECK(vkCreateSemaphore(Device, &SemaphoreInfo,nullptr, &ImageAvailableSems[i]));

		VK_CHECK(vkCreateSemaphore(Device, &SemaphoreInfo,nullptr, &RenderFinishSems[i]));

		VK_CHECK(vkCreateFence(Device,&FenceInfo,nullptr,&InFlightFences[i]));
	}
}

void FRenderer::RecordCommandBuffer(VkCommandBuffer CommandBuffer, uint32 ImageIndex)
{
	if(ImageIndex >= FrameBuffers.size()) return;
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	VK_CHECK(vkBeginCommandBuffer(CommandBuffer, &beginInfo));

	auto SwapChainExtent = SwapChain->GetExtent();
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = RenderPass->RenderPass;
	renderPassInfo.framebuffer = FrameBuffers[ImageIndex]->FrameBuffer;
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = SwapChainExtent;


	VkViewport viewport{};
	viewport.width = (float)SwapChainExtent.width;
	viewport.height = (float)SwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.extent = SwapChainExtent;

	static float grey= 0.5;

	TArray <VkClearValue> clearColor = {
		{.color = {grey, grey, grey, 1.0f} },
		{.depthStencil = {1.0f,0}}
	};

	renderPassInfo.clearValueCount = clearColor.size();
	renderPassInfo.pClearValues = clearColor.data();
	vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo,
	                     VK_SUBPASS_CONTENTS_INLINE);


	vkCmdSetScissor(CommandBuffer, 0, 1, &scissor);
	vkCmdSetViewport(CommandBuffer, 0, 1, &viewport);

	//Primitive->OnRecordCommandBuffer(CommandBuffer,CurrentFrame);
	for(auto P : Primitives)
	{
		if(P)
		{
			P->OnRecordCommandBuffer(CommandBuffer,CurrentFrame);
		}
	}

	vkCmdEndRenderPass(CommandBuffer);
	VK_CHECK(vkEndCommandBuffer(CommandBuffer));
}
FCommandBufferPool *
FRenderer::GetCommandBufferPool() const
{
	return CommandBufferPool.get();
}

void FRenderer::RecreateSwapChains()
{
	if(SwapChain)
	{
		SwapChain->CleanUp();
	}
	SwapChain = std::unique_ptr<FSwapChain>(
		FSwapChain::CreateSwapChain(GRHI->GetDisplaySize()) );

	DepthTextures.resize(SwapChain->GetImageCount()) ;
	for(auto & DepthTexture : DepthTextures) {
		DepthTexture =
			FTexture::CreateTexture<FDepthTexture>({
				                                       0,
				                                       SwapChain->GetExtent().height,
				                                       SwapChain->GetExtent().width
			                                       });
	}
}
bool FRenderer::OnAddPrimitive(SPrimitiveComponent *InComp)
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
bool FRenderer::OnRemovePrimitive(SPrimitiveComponent *InComp)
{
	if(InComp == nullptr) return false;

	if(Primitives.Contains(InComp->AsShared()))
	{
		Primitives.Remove(InComp->AsShared());
		return true;
	}
	return false;
}
