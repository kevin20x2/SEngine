//
// Created by kevin on 2024/8/11.
//

#include "ShaderProgram.h"

#include "RHIUtils.h"

FVertexShaderProgram::FVertexShaderProgram(const char* FilePath)
{
    ShaderModule = FRHIUtils::LoadHlslShaderByFilePath(FilePath, VK_SHADER_STAGE_VERTEX_BIT,DescriptorSetLayoutInfos,VertexInputInfo);
}
FVertexShaderProgram::FVertexShaderProgram(const FString &FilePath) :
	FVertexShaderProgram(FilePath.c_str())
{
	//ShaderModule = FRHIUtils::LoadHlslShaderByFilePath(FilePath, VK_SHADER_STAGE_VERTEX_BIT,DescriptorSetLayoutInfos);
}

FPixelShaderProgram::FPixelShaderProgram(const char* FilePath)
{
	FVertexInputInfo Invalid;
    ShaderModule = FRHIUtils::LoadHlslShaderByFilePath(FilePath,VK_SHADER_STAGE_FRAGMENT_BIT,DescriptorSetLayoutInfos,Invalid);
}
FPixelShaderProgram::FPixelShaderProgram(const FString &FilePath) :
	FPixelShaderProgram(FilePath.c_str())
{
	//ShaderModule = FRHIUtils::LoadHlslShaderByFilePath(FilePath,VK_SHADER_STAGE_FRAGMENT_BIT,DescriptorSetLayoutInfos);
}
