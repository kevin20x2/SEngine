//
// Created by kevin on 2024/8/11.
//

#include "FrameBuffer.h"

#include "RHI.h"
#include "SwapChain.h"
#include "volk.h"
#include "RHI/DepthTexture.h"

FFrameBuffer::FFrameBuffer(int32 ViewIdx,FRenderPass * InRenderPass,
						   FSwapChain* InSwapChain,
						   TArray <TSharedPtr<FDepthTexture>> InDepthTexList
						   )
{
		TArray < VkImageView> attachments = { *InSwapChain->GetView(ViewIdx),InDepthTexList[ViewIdx]->GetImageView()};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = InRenderPass->RenderPass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = InSwapChain->GetExtent().width;
		framebufferInfo.height = InSwapChain->GetExtent().height;
		framebufferInfo.layers = 1;
		VK_CHECK(vkCreateFramebuffer(*GRHI->GetDevice(),
			&framebufferInfo, nullptr,
		                             &FrameBuffer));
}

FFrameBuffer::~FFrameBuffer()
{
	Destroy();
}

void FFrameBuffer::Destroy()
{
	vkDestroyFramebuffer(*GRHI->GetDevice(),FrameBuffer,nullptr);
}
