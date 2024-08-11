//
// Created by kevin on 2024/8/11.
//

#include "FrameBuffer.h"

#include "RHI.h"
#include "SwapChain.h"
#include "volk.h"

FFrameBuffer::FFrameBuffer(int32 ViewIdx,FRenderPass * InRenderPass,FSwapChain* InSwapChain)
{
	//for (size_t i = 0; i < Swa&&pChainImageViews.size(); i++) {
		VkImageView attachments[] = { *InSwapChain->GetView(ViewIdx)};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = InRenderPass->RenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = InSwapChain->GetExtent().width;
		framebufferInfo.height = InSwapChain->GetExtent().height;
		framebufferInfo.layers = 1;
		VK_CHECK(vkCreateFramebuffer(*GRHI->GetDevice(),
			&framebufferInfo, nullptr,
		                             &FrameBuffer));
}
