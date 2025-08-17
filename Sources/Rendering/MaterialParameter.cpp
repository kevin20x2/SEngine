//
// Created by 郑文凯 on 2024/10/2.
//

#include "MaterialParameter.h"

#include "Material.h"
#include "Renderer.h"
#include "RHI/RHI.h"
#include "volk.h"
#include "CoreObjects/Engine.h"
#include "CoreObjects/SObject.h"

TArray<VkWriteDescriptorSet>
FMaterialParameterTexture::GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet)
{
	if(Texture == nullptr) return {} ;

	CachedImageInfo =
		{
		.sampler = Texture->GetSampler(),
		.imageView = Texture->GetImageView(),
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ,
		};

	VkWriteDescriptorSet ImageWrite =
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = DescriptorSet,
			.dstBinding = BindingSlotIdx,
			.dstArrayElement = 0 ,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			.pImageInfo = &CachedImageInfo,
		};

	return {ImageWrite};
}

FMaterialParameterSampler::FMaterialParameterSampler(VkFilter Filter)
{
	Sampler = SNew<FSampler>(Filter);
}

TArray<VkWriteDescriptorSet>
FMaterialParameterSampler::GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet)
{
	//if(Texture == nullptr) return {} ;

	CachedImageInfo =
		{
			.sampler = Sampler->Sampler,
			.imageView = VK_NULL_HANDLE,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ,
		};

	VkWriteDescriptorSet SamplerWrite =
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = DescriptorSet,
			.dstBinding = BindingSlotIdx,
			.dstArrayElement = 0 ,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
			.pImageInfo = &CachedImageInfo
		};

	return {SamplerWrite};
}

FMaterialParameterUniformBuffer::FMaterialParameterUniformBuffer( SMaterialInterface * Material,const FDescriptorSetLayoutBinding& Binding)
{
	if(Binding.BindingInfo)
	{
		Size =  Binding.BindingInfo->Size;
		ParseShaderBindingInfo(Binding.BindingInfo.get());
		auto BufferCount = GRHI->GetMaxFrameInFlight();
		Buffers.resize(BufferCount);
		auto&  DescriptorSets = Material->GetDescriptorSets();
		for(uint32 i = 0 ;i < BufferCount;i++)
		{
			Buffers[i].reset(FUniformBuffer::Create(Size));

			BindingSlotIdx = Binding.Binding.binding;
			BufferInfo =
			{
				.buffer = Buffers[i]->GetBuffer(),
				.offset = 0,
				.range = Size
			};
			VkWriteDescriptorSet DescriptorWrite{};
			DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			DescriptorWrite.dstSet = DescriptorSets[i];
			DescriptorWrite.dstBinding = BindingSlotIdx;
			DescriptorWrite.dstArrayElement = 0;
			DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			DescriptorWrite.descriptorCount = 1;
			DescriptorWrite.pBufferInfo = &BufferInfo;
			vkUpdateDescriptorSets(*GRHI->GetDevice(),1,&DescriptorWrite,0,nullptr);
		}
	}
}

bool FMaterialParameterUniformBuffer::SetVector(const FString& Name, const FVector4& Value)
{
	if(!ContainVector(Name)) return false;

	const auto & Info =  VectorVariables[Name];
	TArray <float > Data = {Value.x,Value.y,Value.z,Value.w};

	auto CurrentFrame = GEngine->GetRenderer()->GetFrameIndex();

	return Buffers[CurrentFrame]->UpdateData(Info.Offset,Info.Size,(void *)Data.data());
}

bool FMaterialParameterUniformBuffer::ContainVector(const FString& Name)
{
	return VectorVariables.Contains(Name);
}

TArray<VkWriteDescriptorSet> FMaterialParameterUniformBuffer::GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet)
{

	return  {};
}

void FMaterialParameterUniformBuffer::ParseShaderBindingInfo(FShaderVariableInfo *Info)
{
	if(Info->Type == EShaderVariableType::Vector)
	{
		VectorVariables[Info->Name] = {
			Info->Name,
			Info->Type,
			Info->Size,
			Info->Offset
		};
	}
    if(Info->Type== EShaderVariableType::Int)
    {
        IntVariables[Info->Name] = {
                Info->Name,
                Info->Type,
                Info->Size,
                Info->Offset
        };
    }
	for(auto & ChildInfo : Info->ChildMembers)
	{
		ParseShaderBindingInfo(ChildInfo.get());
	}
}

bool FMaterialParameterUniformBuffer::ContainInt(const FString &Name)
{
    return IntVariables.Contains(Name);
}

bool FMaterialParameterUniformBuffer::SetInt(const FString &Name, int32 InValue)
{
    if(!ContainInt(Name)) return false;

    const auto & Info =  VectorVariables[Name];
    TArray <int32 > Data = {InValue};

    auto CurrentFrame = GEngine->GetRenderer()->GetFrameIndex();

    return Buffers[CurrentFrame]->UpdateData(Info.Offset,Info.Size,(void *)Data.data());
}


TArray<VkWriteDescriptorSet>
FMaterialParameters::GenerateWriteDescriptorSet(VkDescriptorSet DescriptorSet)
{
	TArray <VkWriteDescriptorSet> Results;
	for(auto Param : Parameters )
	{
		if(Param)
		{
			auto ChildSets =  Param->GenerateWriteDescriptorSets(DescriptorSet);
			Results.insert(Results.end(),ChildSets.begin(),ChildSets.end());
		}
	}
	return Results;
}



void FMaterialParameters::BindParametersToDescriptorSet(VkDescriptorSet DescriptorSet)
{
	auto Writes = GenerateWriteDescriptorSet(DescriptorSet);

	vkUpdateDescriptorSets(*GRHI->GetDevice(),
						   Writes.size(),Writes.data(),0,nullptr);
}
void
FMaterialParameters::SetTexture(uint32 Binding, TSharedPtr<FTextureBase> InTexture)
{
	auto ParamIter =  std::find_if(Parameters.begin(), Parameters.end(),
					   [&] (TSharedPtr <FMaterialParameterBase> Param){
		if(auto TexturePtr =  std::static_pointer_cast<FMaterialParameterTexture>(Param))
		{
			return TexturePtr->BindingSlotIdx == Binding;
		}
		return false;
	});
	if(ParamIter != Parameters.end())
	{
		if(auto TextureParam = std::static_pointer_cast<FMaterialParameterTexture>(*ParamIter))
		{
			TextureParam->Texture = InTexture;
		}
	}
}

void FMaterialParameters::SetTexture(const FString& Name, TSharedPtr<FTextureBase> InTexture)
{

	auto ParamIter =  std::find_if(Parameters.begin(), Parameters.end(),
					   [&] (TSharedPtr <FMaterialParameterBase> Param){
		if(auto TexturePtr =  std::static_pointer_cast<FMaterialParameterTexture>(Param))
		{
			return TexturePtr->Name == Name;
		}
		return false;
	});

	if(ParamIter != Parameters.end())
	{
		if(auto TextureParam = std::static_pointer_cast<FMaterialParameterTexture>(*ParamIter))
		{
			TextureParam->Texture = InTexture;
		}
	}
}

bool FMaterialParameters::SetVector(const FString &Name, const FVector4 &Value)
{
	auto ParamIter =  std::find_if(Parameters.begin(), Parameters.end(),
					   [&] (TSharedPtr <FMaterialParameterBase> Param){
		if(auto Uniform =  std::static_pointer_cast<FMaterialParameterUniformBuffer>(Param))
		{
			return Uniform->ContainVector(Name);
		}
		return false;
	});

	if(ParamIter != Parameters.end())
	{
		if(auto Uniform =  std::static_pointer_cast<FMaterialParameterUniformBuffer>(*ParamIter))
		{
			Uniform->SetVector(Name, Value);
			return true;
		}
	}
	return false;
}

bool FMaterialParameters::SetInt(const FString &Name, int32 IntValue)
{

    auto ParamIter =  std::find_if(Parameters.begin(), Parameters.end(),
                                   [&] (TSharedPtr <FMaterialParameterBase> Param){
                                       if(auto Uniform =  std::static_pointer_cast<FMaterialParameterUniformBuffer>(Param))
                                       {
                                           return Uniform->ContainInt(Name);
                                       }
                                       return false;
                                   });

    if(ParamIter != Parameters.end())
    {
        if(auto Uniform =  std::static_pointer_cast<FMaterialParameterUniformBuffer>(*ParamIter))
        {
            Uniform->SetInt(Name, IntValue);
            return true;
        }
    }
    return false;
}
