//
// Created by 郑文凯 on 2024/10/2.
//

#ifndef MATERIALPARAMETER_H
#define MATERIALPARAMETER_H


#include "Core/BaseTypes.h"
#include "Maths/Math.h"
#include "RHI/ShaderProgram.h"
#include "vulkan/vulkan_core.h"
#include "RHI/Texture.h"
#include "RHI/UniformBuffer.h"

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
	FMaterialParameterSampler(VkFilter InFilter = VK_FILTER_LINEAR);

	TArray < VkWriteDescriptorSet > GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet) override;
	TSharedPtr <FSampler > Sampler;
	VkDescriptorImageInfo CachedImageInfo;
};


class FMaterialParameterUniformBuffer : public FMaterialParameterBase
{
public:

	FMaterialParameterUniformBuffer( const FDescriptorSetLayoutBinding & Binding);

	bool SetVector(const FString & Name, const FVector4 & Value ) ;

	bool ContainVector(const FString & Name);

	virtual TArray <VkWriteDescriptorSet> GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet) override;

protected:

	VkDescriptorBufferInfo BufferInfo ;
	TSharedPtr <FUniformBuffer> Buffer;
	uint32 Size = 0 ;
};

class FMaterialParameters
{
public:
	void BindParametersToDescriptorSet(VkDescriptorSet DescriptorSet);

	void SetTexture(uint32 Binding , TSharedPtr <FTexture> InTexture);

	void SetTexture(const FString & Name, TSharedPtr <FTexture> InTexture);

protected:
	TArray <VkWriteDescriptorSet> GenerateWriteDescriptorSet(VkDescriptorSet DescriptorSet);

public:
	TArray <TSharedPtr <FMaterialParameterBase > > Parameters;
};



#endif //MATERIALPARAMETER_H
