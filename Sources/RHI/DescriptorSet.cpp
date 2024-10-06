//
// Created by kevin on 2024/8/11.
//

#include "DescriptorSet.h"

#include "RHI.h"
#include "volk.h"

FDescriptorSets* FDescriptorSets::Create(const TArray<VkDescriptorSetLayout>& Layouts,
                                         const FDescriptorPool& Pool, const TArray<FUniformBuffer*>& Buffers, TSharedPtr<FTexture> Texture)
{
    FDescriptorSets * Result = new FDescriptorSets();

	uint32 Count = Layouts.size();

	VkDescriptorSetAllocateInfo AllocInfo{};
	AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	AllocInfo.descriptorPool = Pool.Pool;
	AllocInfo.descriptorSetCount = Count;
	AllocInfo.pSetLayouts = Layouts.data();

	Result->DescriptorSets.resize(Count);
	VK_CHECK(vkAllocateDescriptorSets(*GRHI->GetDevice(), &AllocInfo,
		Result->DescriptorSets.data()));

	for (uint32 i = 0; i < Count; ++i)
	{
		VkDescriptorBufferInfo BufferInfo{};
		BufferInfo.buffer = Buffers[i]->Buffer;
		BufferInfo.offset = 0;
		BufferInfo.range = 16 * sizeof(float);

		VkDescriptorImageInfo ImageInfo = {
			.sampler = Texture->GetSampler(),
			.imageView = Texture->GetImageView(),
			.imageLayout =VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		};


		VkWriteDescriptorSet DescriptorWrite{};
		DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		DescriptorWrite.dstSet = Result->DescriptorSets[i];
		DescriptorWrite.dstBinding = 0;
		DescriptorWrite.dstArrayElement = 0;
		DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		DescriptorWrite.descriptorCount = 1;
		DescriptorWrite.pBufferInfo = &BufferInfo;

		VkWriteDescriptorSet ImageWrite =
			{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = Result->DescriptorSets[i],
			.dstBinding = 1,
			.dstArrayElement = 0 ,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			.pImageInfo = & ImageInfo
			};
		VkWriteDescriptorSet SamplerWrite =
			{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = Result->DescriptorSets[i],
				.dstBinding = 2,
				.dstArrayElement = 0 ,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
				.pImageInfo = & ImageInfo
			};

		VkWriteDescriptorSet DstWrites [] = { DescriptorWrite , ImageWrite,SamplerWrite};

		vkUpdateDescriptorSets(*GRHI->GetDevice(),
			2, DstWrites, 0, nullptr);
	}


    return Result;

}
