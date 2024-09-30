//
// Created by kevin on 2024/8/11.
//

#include "DescriptorPool.h"

#include "RHI.h"
#include "volk.h"

FUniformBufferDescriptorPool* FUniformBufferDescriptorPool::Create(uint32 DescriptorCount, uint32 MaxSetNum)
{
    FUniformBufferDescriptorPool * Pool = new FUniformBufferDescriptorPool();

	VkDescriptorPoolSize PoolSize [] = {
		{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = static_cast <uint32_t>(DescriptorCount)
		} ,
		{
			.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = DescriptorCount
		}
	};

	VkDescriptorPoolCreateInfo PoolInfo{};
	PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	PoolInfo.poolSizeCount = 2;
	PoolInfo.pPoolSizes = PoolSize;
	PoolInfo.maxSets = static_cast <uint32_t>(MaxSetNum);
	VK_CHECK(vkCreateDescriptorPool(
		*GRHI->GetDevice()
		, &PoolInfo, nullptr, &Pool->Pool));
    return Pool;
}
