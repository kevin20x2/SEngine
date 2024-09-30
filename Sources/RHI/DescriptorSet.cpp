//
// Created by kevin on 2024/8/11.
//

#include "DescriptorSet.h"

#include "RHI.h"
#include "volk.h"

FDescriptorSets* FDescriptorSets::Create(const TArray<FDescriptorSetLayout*>& Layouts,
                                         const FDescriptorPool& Pool, const TArray<FUniformBuffer*>& Buffers, TSharedPtr<FTexture> Texture)
{
    FDescriptorSets * Result = new FDescriptorSets();

    TArray <VkDescriptorSetLayout > VkLayouts;
    for(auto & Layout : Layouts )
    {
        VkLayouts.push_back(Layout->Layout);
    }
	uint32 Count = Layouts.size();

	VkDescriptorSetAllocateInfo AllocInfo{};
	AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	AllocInfo.descriptorPool = Pool.Pool;
	AllocInfo.descriptorSetCount = Count;
	AllocInfo.pSetLayouts = VkLayouts.data();

	Result->DescriptorSets.resize(Count);
	VK_CHECK(vkAllocateDescriptorSets(*GRHI->GetDevice(), &AllocInfo,
		Result->DescriptorSets.data()));

	for (size_t i = 0; i < Count; ++i)
	{
		VkDescriptorBufferInfo BufferInfo{};
		BufferInfo.buffer = Buffers[i]->Buffer;
		BufferInfo.offset = 0;
		BufferInfo.range = 16 * sizeof(float);

		VkDescriptorImageInfo ImageInfo = {
			.imageLayout =VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			.imageView = Texture->GetImageView(),
			.sampler = Texture->GetSampler()
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
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.pImageInfo = & ImageInfo
			};

		VkWriteDescriptorSet DstWrites [] = { DescriptorWrite , ImageWrite};

		vkUpdateDescriptorSets(*GRHI->GetDevice(),
			2, DstWrites, 0, nullptr);
	}


    return Result;

}
