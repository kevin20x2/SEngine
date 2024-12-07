//
// Created by kevin on 2024/12/7.
//
#include "RenderTargetGroup.h"

#include "RHI.h"
#include "volk.h"

void FRenderTargetGroup::InitializeBySwapChain(FSwapChain* InSwapChain)
{

	CreateParams = {
		InSwapChain->GetExtent().width,
		InSwapChain->GetExtent().height
	};

	const uint32 ImageCount = GRHI->GetMaxFrameInFlight();

	RenderPass = TSharedPtr <FRenderPass>(
		FRenderPass::Create(InSwapChain->GetFormat(),VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) );

    DepthTexture.resize(ImageCount);
    FrameBuffer.resize(ImageCount);

	for (uint32 idx = 0; idx < ImageCount; ++idx)
    {
        DepthTexture[idx] = FTexture::CreateTexture<FDepthTexture>({0,
            CreateParams.Height,CreateParams.Width
        });
        FrameBuffer[idx] = TSharedPtr<FFrameBuffer>(
            new FFrameBuffer(idx, RenderPass.get(),InSwapChain,DepthTexture) );
    }

}

void FRenderTargetGroup::Initialize(FRenderTargetGroupCreateParams& Params)
{

    CreateParams = Params;

    const uint32 ImageCount = GRHI->GetMaxFrameInFlight();

    RenderPass = TSharedPtr <FRenderPass>(
        FRenderPass::Create(VK_FORMAT_R8G8B8A8_SRGB,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) );

    RenderTexture.resize(ImageCount);
    DepthTexture.resize(ImageCount);
    FrameBuffer.resize(ImageCount);

    for(uint32 idx = 0 ; idx < ImageCount; ++idx)
    {
        RenderTexture[idx] = FTexture::CreateTexture<FRenderTexture>({0,
            Params.Height,Params.Width });

        DepthTexture[idx] = FTexture::CreateTexture<FDepthTexture>({0,
            Params.Height,Params.Width
        });
        FrameBuffer[idx] = TSharedPtr<FFrameBuffer>(
            new FFrameBuffer(RenderPass.get(),
                Params.Width,Params.Height,
                RenderTexture[idx]->GetImageView(),
                DepthTexture[idx]->GetImageView() )
            );
    }
}

void FRenderTargetGroup::BeginRenderTargetGroup(VkCommandBuffer CommandBuffer, uint32 ImageIdx, FVector4 InClearColor)
{

	VkExtent2D Extent = {.width = CreateParams.Width,.height = CreateParams.Height };
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = RenderPass->RenderPass;
	renderPassInfo.framebuffer = FrameBuffer[ImageIdx]->FrameBuffer;
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = Extent;


	VkViewport viewport{};
	viewport.width = (float)Extent.width;
	viewport.height = (float)Extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.extent = Extent;


	TArray <VkClearValue> clearColor = {
		{.color = {InClearColor.x, InClearColor.y, InClearColor.z, InClearColor.w} },
		{.depthStencil = {1.0f,0}}
	};

	renderPassInfo.clearValueCount = clearColor.size();
	renderPassInfo.pClearValues = clearColor.data();
	vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo,
	                     VK_SUBPASS_CONTENTS_INLINE);

	vkCmdSetScissor(CommandBuffer, 0, 1, &scissor);
	vkCmdSetViewport(CommandBuffer, 0, 1, &viewport);
}

void FRenderTargetGroup::EndRenderTargetGroup(VkCommandBuffer CommandBuffer)
{
	vkCmdEndRenderPass(CommandBuffer);
	//VK_CHECK(vkEndCommandBuffer(CommandBuffer));
}
