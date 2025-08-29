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
        HostBuffer.resize(Size);
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
	if(!Contain(Name)) return false;

	const auto & Info =  VariableInfos[Name];
	TArray <float > Data = {Value.x,Value.y,Value.z,Value.w};

	auto CurrentFrame = GEngine->GetRenderer()->GetFrameIndex();

	memcpy( (void*)(HostBuffer.data() + Info.Offset),(void *)Data.data(),Info.Size);

	MarkDirty(true);

    return true;
}

bool FMaterialParameterUniformBuffer::Contain(const FString& Name)
{
	return VariableInfos.Contains(Name);
}

TArray<VkWriteDescriptorSet> FMaterialParameterUniformBuffer::GenerateWriteDescriptorSets(VkDescriptorSet DescriptorSet)
{
	return  {};
}

void FMaterialParameterUniformBuffer::ParseShaderBindingInfo(FShaderVariableInfo *Info)
{
	{
		VariableInfos[Info->Name] = {
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



bool FMaterialParameterUniformBuffer::SetInt(const FString &Name, int32 InValue)
{
    if(!Contain(Name)) return false;

    const auto & Info =  VariableInfos[Name];
    TArray <int32 > Data = {InValue};

    memcpy( HostBuffer.data() + Info.Offset,(void *)Data.data(),Info.Size);

	MarkDirty(true);

    return true;
}

bool FMaterialParameterUniformBuffer::GetScalar(const FString &Name, float &OutValue)
{
	if(!Contain(Name)) return false;
	auto & Info = VariableInfos[Name];
	OutValue = * (float * ) (HostBuffer.data()+Info.Offset);
	return true;
}

bool FMaterialParameterUniformBuffer::SetScalar(const FString &Name, float InValue)
{
	if(!Contain(Name) ) return false;

	const auto & Info =  VariableInfos[Name];
	TArray<float> Data= {InValue};
	memcpy(HostBuffer.data() + Info.Offset,(void *)Data.data(),Info.Size);
	MarkDirty(true);
	return true;
}

void FMaterialParameterUniformBuffer::OnSyncToCommandBuffer(VkCommandBuffer CommandBuffer)
{
	for(uint32 FrameIdx = 0 ; FrameIdx < Buffers.size() ; ++FrameIdx)
	{
		FMaterialParameterBase::OnSyncToCommandBuffer(CommandBuffer);
		Buffers[FrameIdx]->UpdateData(CommandBuffer,HostBuffer.data());
	}
	MarkDirty(false);
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
		if(auto TexturePtr =  std::dynamic_pointer_cast<FMaterialParameterTexture>(Param))
		{
			return TexturePtr->BindingSlotIdx == Binding;
		}
		return false;
	});
	if(ParamIter != Parameters.end())
	{
		if(auto TextureParam = std::dynamic_pointer_cast<FMaterialParameterTexture>(*ParamIter))
		{
			TextureParam->Texture = InTexture;
		}
	}
}

void FMaterialParameters::SetTexture(const FString& Name, TSharedPtr<FTextureBase> InTexture)
{

	auto ParamIter =  std::find_if(Parameters.begin(), Parameters.end(),
					   [&] (TSharedPtr <FMaterialParameterBase> Param){
		if(auto TexturePtr =  std::dynamic_pointer_cast<FMaterialParameterTexture>(Param))
		{
			return TexturePtr->Name == Name;
		}
		return false;
	});

	if(ParamIter != Parameters.end())
	{
		if(auto TextureParam = std::dynamic_pointer_cast<FMaterialParameterTexture>(*ParamIter))
		{
			TextureParam->Texture = InTexture;
		}
	}
}

void FMaterialParameters::SetTextureSampler(const FString &Name, TSharedPtr<FTextureBase> InTexture)
{

	auto ParamIter =  std::find_if(Parameters.begin(), Parameters.end(),
					   [&] (TSharedPtr <FMaterialParameterBase> Param){
		if(auto Sampler =  std::dynamic_pointer_cast<FMaterialParameterSampler>(Param))
		{
			return Sampler->Name == Name;
		}
		return false;
	});

	if(ParamIter != Parameters.end())
	{
		if(auto Sampler = std::dynamic_pointer_cast<FMaterialParameterSampler>(*ParamIter))
		{
			Sampler->Sampler->Sampler = InTexture->GetSampler();
		}
	}

}


bool FMaterialParameters::SetVector(const FString &Name, const FVector4 &Value)
{
	auto ParamIter =  std::find_if(Parameters.begin(), Parameters.end(),
					   [&] (TSharedPtr <FMaterialParameterBase> Param){
		if(auto Uniform =  std::dynamic_pointer_cast<FMaterialParameterUniformBuffer>(Param))
		{
			return Uniform->Contain(Name);
		}
		return false;
	});

	if(ParamIter != Parameters.end())
	{
		if(auto Uniform =  std::dynamic_pointer_cast<FMaterialParameterUniformBuffer>(*ParamIter))
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
                                       if(auto Uniform =  std::dynamic_pointer_cast<FMaterialParameterUniformBuffer>(Param))
                                       {
                                           return Uniform->Contain(Name);
                                       }
                                       return false;
                                   });

    if(ParamIter != Parameters.end())
    {
        if(auto Uniform =  std::dynamic_pointer_cast<FMaterialParameterUniformBuffer>(*ParamIter))
        {
            Uniform->SetInt(Name, IntValue);
            return true;
        }
    }
    return false;
}

bool FMaterialParameters::SetScalar(const FString &Name, float InValue)
{
    auto ParamIter =  std::find_if(Parameters.begin(), Parameters.end(),
                                   [&] (TSharedPtr <FMaterialParameterBase> Param){
                                       if(auto Uniform =
                                       	std::dynamic_pointer_cast<FMaterialParameterUniformBuffer>(Param))
                                       {
                                           return Uniform->Contain(Name);
                                       }
                                       return false;
                                   });

    if(ParamIter != Parameters.end())
    {
        if(auto Uniform =
        	std::dynamic_pointer_cast<FMaterialParameterUniformBuffer>(*ParamIter))
        {
            Uniform->SetScalar(Name,InValue );
            return true;
        }
    }
    return false;
}

void FMaterialParameters::OnSyncToCommandBuffer(VkCommandBuffer CommandBuffer)
{
    for(auto Param : Parameters)
    {
        Param->OnSyncToCommandBuffer(CommandBuffer);
    }
}

bool FMaterialParameters::ContainDirtyParameters()
{
	for(auto Param : Parameters)
	{
		if(Param->IsDirty())
		{
			return true;
		}
	}
	return false;
}
