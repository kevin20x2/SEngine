//
// Created by 郑文凯 on 2024/10/1.
//
#pragma once
#include "RHI/ShaderProgram.h"
#include "MaterialParameter.h"


class SMaterialInterface;

class FShader
{
protected:
	friend class SShaderManager;

	FShader(TSharedPtr<FVertexShaderProgram> InVert , TSharedPtr<FPixelShaderProgram> InPixel) :
		VertexShaderProgram(InVert),PixelShaderProgram(InPixel)
	{
		GenerateDescriptorSetLayout();
	}
public:

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

	void SetShaderPath(const FString & Path )
	{
		ShaderPath = Path;
	}
	const FString & GetShaderPath() const
	{
		return ShaderPath;
	}

	void AddUsedMaterial(SMaterialInterface * MaterialInterface);
	void RemoveUsedMaterial(SMaterialInterface * MaterialInterace);

	virtual void OnReCompile();

protected:

	void GenerateDescriptorSetLayout();

	using DescriptorSetLayoutBindingList = TArray <VkDescriptorSetLayoutBinding>;
	TArray <DescriptorSetLayoutBindingList> GenerateLayoutBindings();

	FString ShaderPath;

	TSharedPtr <FVertexShaderProgram> VertexShaderProgram;
	TSharedPtr <FPixelShaderProgram> PixelShaderProgram;
	VkDescriptorSetLayout DescriptorSetLayout;

	TArray <TSharedPtr <SMaterialInterface > > UsedMaterials;
};

