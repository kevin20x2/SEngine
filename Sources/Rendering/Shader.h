//
// Created by 郑文凯 on 2024/10/1.
//
#pragma once
#include "RHI/ShaderProgram.h"
#include "MaterialParameter.h"
#include "Core/Macros.h"
#include "CoreObjects/SObject.h"


class SMaterialInterface;

class SShader : public SObject
{
	S_REGISTER_CLASS(SObject)
protected:
	friend class SShaderManager;

	SShader(TSharedPtr<FVertexShaderProgram> InVert , TSharedPtr<FPixelShaderProgram> InPixel) :
		VertexShaderProgram(InVert),PixelShaderProgram(InPixel)
	{
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

	void GenerateDefaultMaterialParams(FMaterialParameters & MaterialParams, SMaterialInterface * MaterialInterface );

    void GenerateDescriptorSetLayout();

	void AddUsedMaterial(SMaterialInterface * MaterialInterface);
	void RemoveUsedMaterial(SMaterialInterface * MaterialInterace);

	virtual void OnReCompile();

protected:


	using DescriptorSetLayoutBindingList = TArray <FDescriptorSetLayoutBinding>;
	TArray <DescriptorSetLayoutBindingList> GenerateLayoutBindings();

	SPROPERTY(FString, ShaderPath);

	SPROPERTY(FString, Name);

	TSharedPtr <FVertexShaderProgram> VertexShaderProgram;
	TSharedPtr <FPixelShaderProgram> PixelShaderProgram;
	VkDescriptorSetLayout DescriptorSetLayout;

	TArray <TSharedPtr <SMaterialInterface > > UsedMaterials;
};

