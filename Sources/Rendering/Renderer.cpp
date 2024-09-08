//
// Created by kevin on 2024/8/11.
//

#include "Renderer.h"

#include <memory>
#include <spdlog/spdlog.h>

#include "volk.h"
#include "CoreObjects/Engine.h"
#include "Maths/Math.h"
#include "Maths/Vector2.h"
#include "Platform/FbxMeshImporter.h"
#include "RHI/RHI.h"

void OnRawWindowResize(GLFWwindow* Window, int Width, int Height)
{
	auto Renderer = GEngine->GetRenderer();
	GRHI->SetDisplaySize(Width,Height);
	Renderer->OnResize(Window,Width,Height);
}

void FRenderer::Initailize()
{
    SwapChain = std::unique_ptr<FSwapChain>(
        FSwapChain::CreateSwapChain(GRHI->GetDisplaySize()) );

    DescriptorSetLayout = std::unique_ptr<FDescriptorSetLayout>(
        FUniformBufferDescriptorSetLayout::Create() );


    uint32 MaxFrameInFlight = GRHI->GetMaxFrameInFlight();
    UniformBuffers.resize(GRHI->GetMaxFrameInFlight());
    for(auto & UniformBuffer : UniformBuffers)
    {
       UniformBuffer  = TSharedPtr<FUniformBuffer>(
           FUniformBuffer::Create(sizeof(float) * 16)
        );

    }

    DescriptorPool = TUniquePtr<FDescriptorPool>(
        FUniformBufferDescriptorPool::Create(MaxFrameInFlight,MaxFrameInFlight)
        );

    DescriptorSets = TUniquePtr<FDescriptorSets>(
        FDescriptorSets::Create({DescriptorSetLayout.get(),DescriptorSetLayout.get()},*DescriptorPool,
            {UniformBuffers[0].get(),UniformBuffers[1].get()}
            )
        );

    RenderPass = TSharedPtr<FRenderPass>(
        FRenderPass::Create(SwapChain.get())
        );

    VertexShader = std::make_shared<FVertexShaderProgram>("../../shaders/test.vert");
    PixelShader = std::make_shared<FPixelShaderProgram>("../../shaders/test.frag");


	RecreatePipeline();
    CommandBufferPool = TUniquePtr<FCommandBufferPool>(new FCommandBufferPool());

    CommandBuffers = TUniquePtr<FCommandBuffers>(new FCommandBuffers(MaxFrameInFlight,CommandBufferPool.get()));

	RecreateFrameBuffers();

	Primitive = TSharedPtr<SPrimitiveComponent>(new SPrimitiveComponent(nullptr));


	TSharedPtr< FStaticMesh> StaticMesh = TSharedPtr<FStaticMesh>(new FStaticMesh({},{}));
	Primitive->SetStaticMesh(StaticMesh);
	FFbxMeshImporter Importer;
	Importer.ImportMesh("../../Assets/gy.fbx",StaticMesh.get());

	Primitive->CreateRHIResource();
	/*Indexes = {0,1,2};*/
	    CreateSyncObjects();
	Camera = TSharedPtr<SCameraComponent>( new SCameraComponent());
	Camera->SetWorldLocation({-3,0,1});
	//Camera->SetRotation(FQuat(glm::radians(30.0f),FVector(0,1,0)));

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

	FMatrix4 Data (1.0f) ;
	auto ViewMatrix = Camera->GetViewMatrix();
	auto TestView = FVector4(0,0,0,1) * ViewMatrix;
	auto ProjecionMatrix = Camera->GetProjectinMatrix();
	Data = ViewMatrix * ProjecionMatrix;
	//Data = FMatrix4(1.0f);

	//Data.setIdentity();
	UniformBuffers[CurrentFrame]->UpdateData(&Data);

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
	SwapChain->CleanUp();
	SwapChain = std::unique_ptr<FSwapChain>(
        FSwapChain::CreateSwapChain(GRHI->GetDisplaySize()) );
    RenderPass = TSharedPtr<FRenderPass>(
        FRenderPass::Create(SwapChain.get())
        );
	RecreateFrameBuffers();
	RecreatePipeline();
	CommandBuffers->FreeCommandBuffer();
}

void FRenderer::RecreatePipeline()
{
    Pipeline = TUniquePtr<FGraphicsPipeline>(new FGraphicsPipeline(
        {VertexShader.get(),PixelShader.get(), DescriptorSetLayout.get() ,
            RenderPass.get() } ) );
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
            ViewIdx,RenderPass.get(),SwapChain.get()
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
	/*grey += 0.005f;
	if (grey > 1.0f) {
		grey = 0.0f;
	}*/
	VkClearValue clearColor = {{{grey, grey, grey, 1.0f}}};

	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo,
	                     VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
	                  Pipeline->GetHandle());


	vkCmdSetScissor(CommandBuffer, 0, 1, &scissor);
	vkCmdSetViewport(CommandBuffer, 0, 1, &viewport);
	vkCmdSetCullMode(CommandBuffer,VK_CULL_MODE_BACK_BIT);
	vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
	                        Pipeline->GetLayout(), 0, 1, &DescriptorSets->GetHandle()[CurrentFrame],
	                        0, nullptr);

	Primitive->OnRecordCommandBuffer(CommandBuffer);

	/*VkBuffer VertexBuffers[] = {BaseVertexBuffer->GetHandle()};
	VkDeviceSize Offsets[] = {0};
	vkCmdBindVertexBuffers(CommandBuffer,0,1,VertexBuffers,Offsets);
	vkCmdBindIndexBuffer(CommandBuffer,IndexBuffer->GetHandle(),0,VK_INDEX_TYPE_UINT16);

	//vkCmdDraw(CommandBuffer, 3, 1, 0, 0);
	vkCmdDrawIndexed(CommandBuffer,36,1,0,0,0);*/
	vkCmdEndRenderPass(CommandBuffer);
	VK_CHECK(vkEndCommandBuffer(CommandBuffer));
}
