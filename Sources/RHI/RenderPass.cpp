//
// Created by kevin on 2024/8/11.
//

#include "RenderPass.h"

#include "RHI.h"
#include "volk.h"
#include "RHIUtils.h"

FRenderPass* FRenderPass::Create(VkFormat InFormat,VkImageLayout ImageLayout,uint32 ColorCount,bool bNeedDepth)
{
	FRenderPass * Result = new FRenderPass;

	Result->ColorAttachmentNum = ColorCount;
	TArray < VkAttachmentDescription> Attachments;
	for(uint32 Idx = 0 ; Idx <ColorCount ; ++ Idx)
	{
		VkAttachmentDescription colorAttachment;
        colorAttachment.flags = 0;
		colorAttachment.format = InFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = ImageLayout;

		Attachments.Add(colorAttachment);
	}

	VkAttachmentDescription depthAttachment{};
    depthAttachment.flags = 0;
	depthAttachment.format = FRHIUtils::FindDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference DepthAttachmentRef {
		.attachment = ColorCount,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	TArray <VkAttachmentReference> colorAttachmentRefs{};
	for(uint32 Idx = 0 ;Idx < ColorCount ; ++Idx)
	{
		VkAttachmentReference ColorAttachmentRef ;
		ColorAttachmentRef.attachment = Idx;
		ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachmentRefs.Add(ColorAttachmentRef);
	}

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = ColorCount;
	subpass.pColorAttachments = colorAttachmentRefs.data();
	subpass.pDepthStencilAttachment = bNeedDepth ?  & DepthAttachmentRef : nullptr;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	Attachments.Add(depthAttachment);

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = ColorCount + ( bNeedDepth ?  1 : 0) ;
	renderPassInfo.pAttachments = Attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VK_CHECK(vkCreateRenderPass(
		*GRHI->GetDevice(),
		&renderPassInfo, nullptr,
		& Result->RenderPass));
	return Result;
}

FRenderPass::~FRenderPass()
{
	vkDestroyRenderPass(*GRHI->GetDevice(),RenderPass,nullptr);
}
