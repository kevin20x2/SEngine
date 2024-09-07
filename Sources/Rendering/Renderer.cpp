//
// Created by kevin on 2024/8/11.
//

#include "Renderer.h"

#include <memory>

#include "volk.h"
#include "Maths/Math.h"
#include "Maths/Vector2.h"
#include "RHI/RHI.h"

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


    Pipeline = TUniquePtr<FGraphicsPipeline>(new FGraphicsPipeline(
        {VertexShader.get(),PixelShader.get(), DescriptorSetLayout.get() ,
            RenderPass.get() } ) );

    CommandBufferPool = TUniquePtr<FCommandBufferPool>(new FCommandBufferPool());

    CommandBuffers = TUniquePtr<FCommandBuffers>(new FCommandBuffers(MaxFrameInFlight,CommandBufferPool.get()));

    FrameBuffers.resize(MaxFrameInFlight);

    int32 ViewIdx = 0;
    for(auto & FrameBuffer : FrameBuffers )
    {
        FrameBuffer = TSharedPtr <FFrameBuffer>( new FFrameBuffer(
            ViewIdx,RenderPass.get(),SwapChain.get()
            ));
        ViewIdx ++ ;
    }

	TArray <FVector> Vertices = {
    	{-1.0f,-1.0f,-1.0f}, {-1.0f,-1.0f,1.0f} ,
    	{-1.0f,1.0f,-1.0f}, {-1.0f,1.0f,1.0f} ,

    	{1.0f,-1.0f,-1.0f}, {1.0f,-1.0f,1.0f} ,
    	{1.0f,1.0f,-1.0f}, {1.0f,1.0f,1.0f} ,
	};
	//printf("sizeof %d\n",sizeof(FVector));
	/*Vertices = {{0,0,0 }, {1,1,0.f}, {0,1.0f,0.0f } };
	float * Array = (float * ) Vertices.data();
	printf("\n");
	for(int32 i = 0 ; i< 9  ;++i)
	{
		printf("%f ", Array[i]);
	}*/
	printf("\n");
	BaseVertexBuffer = TSharedPtr<FVertexBuffer>(
		new FVertexBuffer({(uint32)(Vertices.size()* sizeof(FVector)), (float *)Vertices.data()})
		);

	TArray <uint16> Indexes = {
		0,1,2 , // front
		1,3,2 ,
		4,1,0, // left
		4,5,1,
		2,3,6, // right
		3,7,6 ,
		0,4,2 , // bottom
		2,4,6 ,
		1,5,3, // top
		3,5,7,
		5,4,6, // back
		7,5,6
	};
	/*Indexes = {0,1,2};*/
	IndexBuffer = TSharedPtr<FIndexBuffer>(
		new FIndexBuffer({(uint32)(Indexes.size()* sizeof(uint16)), (uint16 *)Indexes.data()}));

    CreateSyncObjects();
	Camera = new SCameraComponent();
	Camera->SetWorldLocation({-5,0,1});
	Camera->SetRotation(FQuat(glm::radians(30.0f),FVector(0,1,0)));

	for(auto V : Vertices)
	{
		auto VP = Camera->GetViewMatrix() * Camera->GetProjectinMatrix();
		auto ClipP =  FVector4(V,1.0f)  *VP;
		printf("%f %f %f %f\n",ClipP.x,ClipP.y,ClipP.z,ClipP.w);
	}
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

	static float grey;
	grey += 0.005f;
	if (grey > 1.0f) {
		grey = 0.0f;
	}
	VkClearValue clearColor = {{{grey, grey, grey, 1.0f}}};

	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo,
	                     VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
	                  Pipeline->GetHandle());

	VkBuffer VertexBuffers[] = {BaseVertexBuffer->GetHandle()};

	VkDeviceSize Offsets[] = {0};
	vkCmdBindVertexBuffers(CommandBuffer,0,1,VertexBuffers,Offsets);
	vkCmdBindIndexBuffer(CommandBuffer,IndexBuffer->GetHandle(),0,VK_INDEX_TYPE_UINT16);
	vkCmdSetScissor(CommandBuffer, 0, 1, &scissor);
	vkCmdSetViewport(CommandBuffer, 0, 1, &viewport);
	vkCmdSetCullMode(CommandBuffer,VK_CULL_MODE_NONE);
	vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
	                        Pipeline->GetLayout(), 0, 1, &DescriptorSets->GetHandle()[CurrentFrame],
	                        0, nullptr);

	//vkCmdDraw(CommandBuffer, 3, 1, 0, 0);
	vkCmdDrawIndexed(CommandBuffer,36,1,0,0,0);
	vkCmdEndRenderPass(CommandBuffer);
	VK_CHECK(vkEndCommandBuffer(CommandBuffer));
}
