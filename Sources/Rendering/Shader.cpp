//
// Created by 郑文凯 on 2024/10/1.
//

#include "Shader.h"

#include "Material.h"
#include "volk.h"
#include "RHI/RHI.h"


void SShader::AddUsedMaterial(SMaterialInterface* MaterialInterface)
{
	UsedMaterials.AddUnique(MaterialInterface->AsShared());
}

void SShader::RemoveUsedMaterial(SMaterialInterface* MaterialInterace)
{
	UsedMaterials.Remove(MaterialInterace->AsShared());
}

void SShader::OnReCompile()
{
	if(VertexShaderProgram)
	{
		VertexShaderProgram->ReCompile();
	}
	if(PixelShaderProgram)
	{
		PixelShaderProgram->ReCompile();
	}
	for(auto Material : UsedMaterials)
	{
		if(Material)
		{
			Material->MarkDirty();
		}
	}
}

void SShader::GenerateDescriptorSetLayout()
{
	if(VertexShaderProgram == nullptr || PixelShaderProgram == nullptr)
	{
		return ;
	}
	auto & VertexInfos = VertexShaderProgram->GetDescriptorSetLayoutInfos();
	auto & PixelInfos = PixelShaderProgram->GetDescriptorSetLayoutInfos();

	if(VertexInfos.size() != PixelInfos.size())
	{
		return ;
	}

	for(uint32 i = 0 ; i < VertexInfos.size(); ++ i)
	{
		TArray <VkDescriptorSetLayoutBinding> Bindings;
		VkDescriptorSetLayoutCreateInfo CreateInfo = VertexInfos[i].CreateInfo;
		for(auto & Binding : VertexInfos[i].Bindings)
		{
			if(Bindings.FindByPredict([&](const VkDescriptorSetLayoutBinding & It )
			{
				 return  It.binding == Binding.binding;
			})== Bindings.end())
			{
				Bindings.push_back(Binding);
			}
		}

		for(auto & Binding : PixelInfos[i].Bindings)
		{
			if (auto Iter = Bindings.FindByPredict([&](const VkDescriptorSetLayoutBinding& It)
			{
				 return  It.binding == Binding.binding;
			}) ; Iter == Bindings.end())
			{
				Bindings.push_back(Binding);
			}
			else
			{
				Iter->stageFlags |=  Binding.stageFlags;
			}
		}

		CreateInfo.bindingCount = Bindings.size();
		CreateInfo.pBindings = Bindings.data();

		VK_CHECK(vkCreateDescriptorSetLayout(*GRHI->GetDevice(),
									&CreateInfo, nullptr,&DescriptorSetLayout));
	}
}
void
SShader::GenerateDefaultMaterialParams(FMaterialParameters &MaterialParams)
{
	auto BindingListList = GenerateLayoutBindings();
	uint32 DescriptorIdx = 0 ;
	for(auto & BindingList : BindingListList)
	{
		for(auto & Binding : BindingList )
		{
			if(Binding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
			{
				auto & Texture = MaterialParams.Parameters.emplace_back(TSharedPtr<FMaterialParameterTexture>(
					new FMaterialParameterTexture()
					));
				Texture->DescriptorIdx = DescriptorIdx;
				Texture->BindingSlotIdx = Binding.binding;
			}
			if(Binding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
			{
				auto & Sampler = MaterialParams.Parameters.emplace_back(TSharedPtr <FMaterialParameterSampler>(
					new FMaterialParameterSampler()
					));
				Sampler->DescriptorIdx = DescriptorIdx;
				Sampler->BindingSlotIdx = Binding.binding;
			}
		}
		DescriptorIdx ++ ;
	}
}
TArray<SShader::DescriptorSetLayoutBindingList>
SShader::GenerateLayoutBindings()
{

	if(VertexShaderProgram == nullptr || PixelShaderProgram == nullptr)
	{
		return {};
	}
	auto & VertexInfos = VertexShaderProgram->GetDescriptorSetLayoutInfos();
	auto & PixelInfos = PixelShaderProgram->GetDescriptorSetLayoutInfos();

	if(VertexInfos.size() != PixelInfos.size())
	{
		return {};
	}

	TArray<SShader::DescriptorSetLayoutBindingList> Results;

	for(uint32 i = 0 ; i < VertexInfos.size(); ++ i)
	{
		TArray <VkDescriptorSetLayoutBinding> Bindings;
		for(auto & Binding : VertexInfos[i].Bindings)
		{
			Bindings.push_back(Binding);
		}

		for(auto & Binding : PixelInfos[i].Bindings)
		{
			Bindings.push_back(Binding);
		}
		Results.push_back(Bindings);
	}
	return Results;
}
