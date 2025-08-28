//
// Created by kevin on 2024/8/11.
//

#include "ShaderProgram.h"

#include "RHIUtils.h"
#include "volk.h"
#include "Core/Log.h"

FVertexShaderProgram::FVertexShaderProgram(const char* FilePath)
{
	ShaderFilePath = FilePath;
    ShaderModule = FRHIUtils::LoadHlslShaderByFilePath(FilePath, VK_SHADER_STAGE_VERTEX_BIT,DescriptorSetLayoutInfos,VertexInputInfo);
	EntryPointName = FRHIUtils::GetShaderEntryPointByShaderStage(VK_SHADER_STAGE_VERTEX_BIT);

	SLogW("DescriptorLayoutInfos {}",DescriptorSetLayoutInfos.size() );

}



void FVertexShaderProgram::ReCompile()
{
	FShaderProgram::ReCompile();
	Destroy();
    ShaderModule = FRHIUtils::LoadHlslShaderByFilePath(ShaderFilePath,
    	VK_SHADER_STAGE_VERTEX_BIT,DescriptorSetLayoutInfos,VertexInputInfo);
}

FVertexShaderProgram::~FVertexShaderProgram()
{
	Destroy();
}

FVertexShaderProgram::FVertexShaderProgram(const FString &FilePath) :
	FVertexShaderProgram(FilePath.c_str())
{
}

void FVertexShaderProgram::Destroy()
{
	vkDestroyShaderModule(*GRHI->GetDevice(),ShaderModule,nullptr);
}


void FPixelShaderProgram::ReCompile()
{
	FShaderProgram::ReCompile();
	FVertexInputInfo Invalid;
	Destroy();
    ShaderModule = FRHIUtils::LoadHlslShaderByFilePath(ShaderFilePath,VK_SHADER_STAGE_FRAGMENT_BIT,DescriptorSetLayoutInfos,Invalid);
}

FPixelShaderProgram::~FPixelShaderProgram()
{
	Destroy();
}

FPixelShaderProgram::FPixelShaderProgram(const char* FilePath)
{
	ShaderFilePath = FilePath;
	FVertexInputInfo Invalid;
    ShaderModule = FRHIUtils::LoadHlslShaderByFilePath(FilePath,VK_SHADER_STAGE_FRAGMENT_BIT,DescriptorSetLayoutInfos,Invalid);
	EntryPointName = FRHIUtils::GetShaderEntryPointByShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT);
	SLogW("SetLayout Infos {}", DescriptorSetLayoutInfos.size());
}
FPixelShaderProgram::FPixelShaderProgram(const FString &FilePath) :
	FPixelShaderProgram(FilePath.c_str())
{
	//ShaderModule = FRHIUtils::LoadHlslShaderByFilePath(FilePath,VK_SHADER_STAGE_FRAGMENT_BIT,DescriptorSetLayoutInfos);
}

void FPixelShaderProgram::Destroy()
{
	vkDestroyShaderModule(*GRHI->GetDevice(),ShaderModule,nullptr);
}
