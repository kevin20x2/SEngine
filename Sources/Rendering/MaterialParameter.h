//
// Created by 郑文凯 on 2024/10/2.
//

#ifndef MATERIALPARAMETER_H
#define MATERIALPARAMETER_H


#include "Core/BaseTypes.h"
#include "Maths/Math.h"
#include "RHI/ShaderProgram.h"
#include "vulkan/vulkan_core.h"
#include "RHI/Texture2D.h"
#include "RHI/UniformBuffer.h"

class SMaterialInterface;

class FMaterialParameterBase
{
public:
	virtual TArray < VkWriteDescriptorSet > GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet) = 0;

	FString Name;
	uint32 DescriptorIdx ;
	uint32 BindingSlotIdx ;

    virtual void OnSyncToCommandBuffer(VkCommandBuffer CommandBuffer)
    { }

	virtual bool IsDirty() const
    {
    	return false;
    }

};


class FMaterialParameterTexture : public FMaterialParameterBase
{
public:
	TArray < VkWriteDescriptorSet > GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet) override;
	TSharedPtr <FTextureBase > Texture;
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

	struct FVariableInfo
	{
		FString Name;
		EShaderVariableType Type;
		uint32 Size;
		uint32 Offset;
	};


	FMaterialParameterUniformBuffer(SMaterialInterface * Material ,const FDescriptorSetLayoutBinding & Binding);

	bool SetVector(const FString & Name, const FVector4 & Value ) ;

	bool Contain(const FString & Name);


    bool SetInt(const FString & Name, int32 InValue);

	bool GetScalar(const FString & Name, float & OutValue);

	bool SetScalar(const FString & Name,float InValue);

	virtual TArray <VkWriteDescriptorSet> GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet) override;

    virtual void OnSyncToCommandBuffer(VkCommandBuffer CommandBuffer) override;

	virtual bool IsDirty() const override
	{
		return bDirty;
	}

	void MarkDirty(bool InValue)
	{
		bDirty = InValue;
	}
	const TMap <FString , FVariableInfo> & GetVariableInfos() const
	{
		return VariableInfos;
	}

protected:
	void ParseShaderBindingInfo(FShaderVariableInfo * Info);


	TMap <FString,FVariableInfo > VariableInfos;


    TArray <uint8> HostBuffer;

	VkDescriptorBufferInfo BufferInfo ;
	TArray <TSharedPtr <FUniformBuffer> > Buffers;
	uint32 Size = 0 ;

	bool bDirty = false;

};

class FMaterialParameters
{
public:
	void BindParametersToDescriptorSet(VkDescriptorSet DescriptorSet);

	void SetTexture(uint32 Binding , TSharedPtr <FTextureBase> InTexture);

	void SetTexture(const FString & Name, TSharedPtr <FTextureBase> InTexture);

	void SetTextureSampler(const FString & Name , TSharedPtr <FTextureBase> InTexture );

	bool SetVector(const FString & Name, const FVector4 & Value);

    bool SetInt(const FString & Name,int32 IntValue);

	bool SetScalar(const FString & Name,float InValue);

    void OnSyncToCommandBuffer(VkCommandBuffer CommandBuffer);

	bool ContainDirtyParameters();

protected:
	TArray <VkWriteDescriptorSet> GenerateWriteDescriptorSet(VkDescriptorSet DescriptorSet);

public:
	TArray <TSharedPtr <FMaterialParameterBase > > Parameters;
};



#endif //MATERIALPARAMETER_H
