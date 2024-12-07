//
// Created by 郑文凯 on 2024/10/2.
//

#include "MaterialParameter.h"
#include "RHI/RHI.h"
#include "volk.h"
#include "CoreObjects/SObject.h"

TArray<VkWriteDescriptorSet>
FMaterialParameterTexture::GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet)
{
	if(Texture == nullptr) return {} ;

	CachedImageInfo =
		{
		.sampler = Texture->GetSampler(),
		.imageView = Texture->GetImageView(),
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ,
		};

	VkWriteDescriptorSet ImageWrite =
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = DescriptorSet,
			.dstBinding = BindingSlotIdx,
			.dstArrayElement = 0 ,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			.pImageInfo = &CachedImageInfo,
		};

	return {ImageWrite};
}

FMaterialParameterSampler::FMaterialParameterSampler(VkFilter Filter)
{
	Sampler = SNew<FSampler>(Filter);
}

TArray<VkWriteDescriptorSet>
FMaterialParameterSampler::GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet)
{
	//if(Texture == nullptr) return {} ;

	CachedImageInfo =
		{
			.sampler = Sampler->Sampler,
			.imageView = VK_NULL_HANDLE,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ,
		};

	VkWriteDescriptorSet SamplerWrite =
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = DescriptorSet,
			.dstBinding = BindingSlotIdx,
			.dstArrayElement = 0 ,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
			.pImageInfo = &CachedImageInfo
		};

	return {SamplerWrite};
}
TArray<VkWriteDescriptorSet>
FMaterialParameters::GenerateWriteDescriptorSet(VkDescriptorSet DescriptorSet)
{
	TArray <VkWriteDescriptorSet> Results;
	for(auto Param : Parameters )
	{
		if(Param)
		{
			auto ChildSets =  Param->GenerateWriteDescriptorSets(DescriptorSet);
			Results.insert(Results.end(),ChildSets.begin(),ChildSets.end());
		}
	}
	return Results;
}

void FMaterialParameters::BindParametersToDescriptorSet(VkDescriptorSet DescriptorSet)
{
	auto Writes = GenerateWriteDescriptorSet(DescriptorSet);

	vkUpdateDescriptorSets(*GRHI->GetDevice(),
						   Writes.size(),Writes.data(),0,nullptr);
}
void
FMaterialParameters::SetTexture(uint32 Binding, TSharedPtr<FTexture> InTexture)
{
	auto ParamIter =  std::find_if(Parameters.begin(), Parameters.end(),
					   [&] (TSharedPtr <FMaterialParameterBase> Param){
		if(auto TexturePtr =  std::static_pointer_cast<FMaterialParameterTexture>(Param))
		{
			return TexturePtr->BindingSlotIdx == Binding;
		}
		return false;
	});
	if(ParamIter != Parameters.end())
	{
		if(auto TextureParam = std::static_pointer_cast<FMaterialParameterTexture>(*ParamIter))
		{
			TextureParam->Texture = InTexture;
		}
	}
}

void FMaterialParameters::SetTexture(const FString& Name, TSharedPtr<FTexture> InTexture)
{

	auto ParamIter =  std::find_if(Parameters.begin(), Parameters.end(),
					   [&] (TSharedPtr <FMaterialParameterBase> Param){
		if(auto TexturePtr =  std::static_pointer_cast<FMaterialParameterTexture>(Param))
		{
			return TexturePtr->Name == Name;
		}
		return false;
	});

	if(ParamIter != Parameters.end())
	{
		if(auto TextureParam = std::static_pointer_cast<FMaterialParameterTexture>(*ParamIter))
		{
			TextureParam->Texture = InTexture;
		}
	}
}
