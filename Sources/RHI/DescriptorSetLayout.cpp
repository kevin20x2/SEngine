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

	VkDescriptorSetLayoutBinding SamplerBinding=
		{
			.binding = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			.pImmutableSamplers = nullptr
		};

	VkDescriptorSetLayoutBinding Bindings[] =
		{UboLayoutBinding,SamplerBinding};

	VkDescriptorSetLayoutCreateInfo LayoutCreateInfo{};
	LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	LayoutCreateInfo.bindingCount = 2;
	LayoutCreateInfo.pBindings = Bindings;

	VK_CHECK(vkCreateDescriptorSetLayout(
		*GRHI->GetDevice()
		,&LayoutCreateInfo,nullptr,&Layout));
}

FImageSamplerDescriptorSetLayout *
FImageSamplerDescriptorSetLayout::Create()
{
	auto Layout = new FImageSamplerDescriptorSetLayout;
	Layout->Init();
	return Layout;
}
void
FImageSamplerDescriptorSetLayout::Init()
{
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

}
