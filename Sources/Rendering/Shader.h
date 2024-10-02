//
// Created by 郑文凯 on 2024/10/1.
//
#pragma once
#include "RHI/ShaderProgram.h"
#include "MaterialParameter.h"


class FShader
{
public:
	FShader(TSharedPtr<FVertexShaderProgram> InVert , TSharedPtr<FPixelShaderProgram> InPixel) :
		VertexShaderProgram(InVert),PixelShaderProgram(InPixel)
	{
		GenerateDescriptorSetLayout();
	}

	VkDescriptorSetLayout GetDescriptorSetLayout()
	{
		return DescriptorSetLayout;
	}
	VkDescriptorSetLayout & GetDescriptorSetLayoutRef()
	{
		return DescriptorSetLayout;
	}

	FVertexShaderProgram * GetVertexShader()
	{
		return VertexShaderProgram.get();
	}
	FPixelShaderProgram * GetPixelShader()
	{
		return PixelShaderProgram.get();
	}

	const FVertexInputInfo & GetVertexInputInfo()
	{
		return VertexShaderProgram->VertexInputInfo;
	}

	void GenerateDefaultMaterialParams(FMaterialParameters & MaterialParams );

protected:

	void GenerateDescriptorSetLayout();

	using DescriptorSetLayoutBindingList = TArray <VkDescriptorSetLayoutBinding>;
	TArray <DescriptorSetLayoutBindingList> GenerateLayoutBindings();

	FString ShaderPath;

	TSharedPtr <FVertexShaderProgram> VertexShaderProgram;
	TSharedPtr <FPixelShaderProgram> PixelShaderProgram;
	VkDescriptorSetLayout DescriptorSetLayout;
};

