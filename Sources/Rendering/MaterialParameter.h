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

class SMaterialInterface;

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

	FMaterialParameterUniformBuffer(SMaterialInterface * Material ,const FDescriptorSetLayoutBinding & Binding);

	bool SetVector(const FString & Name, const FVector4 & Value ) ;

	bool ContainVector(const FString & Name);

	virtual TArray <VkWriteDescriptorSet> GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet) override;

protected:
	void ParseShaderBindingInfo(FShaderVariableInfo * Info);

	struct FVariableInfo
	{
		FString Name;
		EShaderVariableType Type;
		uint32 Size;
		uint32 Offset;
	};

	TMap <FString,FVariableInfo > VectorVariables;

	VkDescriptorBufferInfo BufferInfo ;
	TArray <TSharedPtr <FUniformBuffer> > Buffers;
	uint32 Size = 0 ;
};

class FMaterialParameters
{
public:
	void BindParametersToDescriptorSet(VkDescriptorSet DescriptorSet);

	void SetTexture(uint32 Binding , TSharedPtr <FTexture> InTexture);

	void SetTexture(const FString & Name, TSharedPtr <FTexture> InTexture);

	bool SetVector(const FString & Name, const FVector4 & Value);

protected:
	TArray <VkWriteDescriptorSet> GenerateWriteDescriptorSet(VkDescriptorSet DescriptorSet);

public:
	TArray <TSharedPtr <FMaterialParameterBase > > Parameters;
};



#endif //MATERIALPARAMETER_H
