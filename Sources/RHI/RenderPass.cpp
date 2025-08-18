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


	TArray<VkSubpassDependency> dependencies;
	dependencies.resize(2);
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	Attachments.Add(depthAttachment);

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = ColorCount + ( bNeedDepth ?  1 : 0) ;
	renderPassInfo.pAttachments = Attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = dependencies.size();
	renderPassInfo.pDependencies = dependencies.data();

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
