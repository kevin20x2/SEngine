//
// Created by 郑文凯 on 2024/10/2.
//

#ifndef MATERIALPARAMETER_H
#define MATERIALPARAMETER_H


#include "Core/BaseTypes.h"
#include "vulkan/vulkan_core.h"
#include "RHI/Texture.h"

class FMaterialParameterBase
{
public:
	virtual TArray < VkWriteDescriptorSet > GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet) = 0;

	FString Name;
	uint32 DescriptorIdx ;
	uint32 BindingSlotIdx ;
};


class FMaterialParameterTexture : public FMaterialParameterBase
{
public:
	TArray < VkWriteDescriptorSet > GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet) override;
	TSharedPtr <FTexture > Texture;
	VkDescriptorImageInfo CachedImageInfo;
};

class FMaterialParameterSampler : public FMaterialParameterBase
{
public:
	TArray < VkWriteDescriptorSet > GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet) override;
	TSharedPtr <FTexture > Texture;
	VkDescriptorImageInfo CachedImageInfo;
};

class FMaterialParameters
{
public:
	void BindParametersToDescriptorSet(VkDescriptorSet DescriptorSet);

	void SetTexture(uint32 Binding , TSharedPtr <FTexture> InTexture);

protected:
	TArray <VkWriteDescriptorSet> GenerateWriteDescriptorSet(VkDescriptorSet DescriptorSet);

public:
	TArray <TSharedPtr <FMaterialParameterBase > > Parameters;
};



#endif //MATERIALPARAMETER_H
