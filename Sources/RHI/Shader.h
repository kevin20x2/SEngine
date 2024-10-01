//
// Created by kevin on 2024/8/11.
//

#ifndef SHADER_H
#define SHADER_H
#include <vulkan/vulkan_core.h>
#include "Core/BaseTypes.h"


struct FDescriptorSetLayoutInfo
{
	uint32 SetNumber;
	VkDescriptorSetLayoutCreateInfo CreateInfo;
	TArray <VkDescriptorSetLayoutBinding> Bindings;
};

class FShaderProgram
{
public:
    VkShaderModule GetShaderModule() const
    {
        return ShaderModule;
    }
protected:
    VkShaderModule ShaderModule;

	TArray <FDescriptorSetLayoutInfo> DescriptorSetLayoutInfos;
};

class FVertexShaderProgram : public FShaderProgram
{
public:
	explicit FVertexShaderProgram(const FString & FilePath);
    explicit FVertexShaderProgram(const char * FilePath );
};

class FPixelShaderProgram : public FShaderProgram
{
public:
    explicit FPixelShaderProgram(const char * FilePath);
	explicit FPixelShaderProgram(const FString & FilePath);
};

#endif //SHADER_H
