//
// Created by kevin on 2024/8/11.
//

#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H
#include <vulkan/vulkan_core.h>
#include "Core/BaseTypes.h"


struct FDescriptorSetLayoutInfo
{
	uint32 SetNumber;
	VkDescriptorSetLayoutCreateInfo CreateInfo;
	TArray <VkDescriptorSetLayoutBinding> Bindings;
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

protected:
    VkShaderModule ShaderModule;

	FString EntryPointName ;

	TArray <FDescriptorSetLayoutInfo> DescriptorSetLayoutInfos;

	FString ShaderFilePath;
};

class FVertexShaderProgram : public FShaderProgram
{
public:
	explicit FVertexShaderProgram(const FString & FilePath);
    explicit FVertexShaderProgram(const char * FilePath );
	FVertexInputInfo VertexInputInfo;
};

class FPixelShaderProgram : public FShaderProgram
{
public:
    explicit FPixelShaderProgram(const char * FilePath);
	explicit FPixelShaderProgram(const FString & FilePath);
};

#endif //SHADERPROGRAM_H
