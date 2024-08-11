//
// Created by kevin on 2024/8/11.
//

#include "DescriptorSetLayout.h"

#include "RHI.h"
#include "volk.h"


FUniformBufferDescriptorSetLayout* FUniformBufferDescriptorSetLayout::Create()
{
    FUniformBufferDescriptorSetLayout * Layout = new FUniformBufferDescriptorSetLayout;
    Layout->Init();
    return Layout;
}

void FUniformBufferDescriptorSetLayout::Init()
{
	VkDescriptorSetLayoutBinding UboLayoutBinding{};
	UboLayoutBinding.binding = 0;
	UboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	UboLayoutBinding.descriptorCount = 1;
	UboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	UboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo LayoutCreateInfo{};
	LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	LayoutCreateInfo.bindingCount = 1;
	LayoutCreateInfo.pBindings = &UboLayoutBinding;

	VK_CHECK(vkCreateDescriptorSetLayout(
		*GRHI->GetDevice()
		,&LayoutCreateInfo,nullptr,&Layout));
}
