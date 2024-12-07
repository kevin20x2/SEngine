//
// Created by kevin on 2024/8/11.
//

#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H
#include <vulkan/vulkan_core.h>
#include "Core/BaseTypes.h"


struct FDescriptorSetLayoutBinding
{
	FString Name;
	VkDescriptorSetLayoutBinding Binding;
};

struct FDescriptorSetLayoutInfo
{
	uint32 SetNumber;
	//VkDescriptorSetLayoutCreateInfo CreateInfo;
	TArray <FDescriptorSetLayoutBinding> Bindings;
};

struct FVertexInputInfo
{
	VkPipelineVertexInputStateCreateInfo CreateInfo;
	TArray <VkVertexInputBindingDescription> BindingDescription;
	TArray <VkVertexInputAttributeDescription> AttributeDescription;
};

class FShaderProgram : public std::enable_shared_from_this<FShaderProgram>
{
public:
    VkShaderModule GetShaderModule() const
    {
        return ShaderModule;
    }
	const TArray<FDescriptorSetLayoutInfo> & GetDescriptorSetLayoutInfos()
	{
		return DescriptorSetLayoutInfos;
	}

	const FString & GetShaderFilePath() const
    {
    	return ShaderFilePath;
    }
	const char * GetEntryPointName() const
    {
    	return EntryPointName.c_str();
    }

	virtual void ReCompile() {} ;

protected:
    VkShaderModule ShaderModule;

	FString EntryPointName ;

	TArray <FDescriptorSetLayoutInfo> DescriptorSetLayoutInfos;

	FString ShaderFilePath;
};

class FVertexShaderProgram : public FShaderProgram
{
public:
	virtual void ReCompile() override;


	virtual ~FVertexShaderProgram();

	explicit FVertexShaderProgram(const FString & FilePath);
    explicit FVertexShaderProgram(const char * FilePath );
	FVertexInputInfo VertexInputInfo;
protected:
	void Destroy();
};

class FPixelShaderProgram : public FShaderProgram
{
public:
	virtual void ReCompile() override;
	virtual ~FPixelShaderProgram();
    explicit FPixelShaderProgram(const char * FilePath);
	explicit FPixelShaderProgram(const FString & FilePath);
protected:
	void Destroy();
};

#endif //SHADERPROGRAM_H
