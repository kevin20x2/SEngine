//
// Created by 郑文凯 on 2024/10/2.
//

#include "Material.h"

#include "LightRenderData.h"
#include "Renderer.h"
#include "RHI/RHI.h"
#include "volk.h"
#include "CoreObjects/Engine.h"

void SMaterialInterface::CreateDescriptionSets(VkDescriptorPool Pool)
{
	if(Shader == nullptr) return;
	uint32 Count = GRHI->GetMaxFrameInFlight();
	TArray <VkDescriptorSetLayout> Layouts(Count,Shader->GetDescriptorSetLayout());

	VkDescriptorSetAllocateInfo AllocInfo=
		{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = Pool,
		.descriptorSetCount = Count ,
		.pSetLayouts =Layouts.data()
		};

	DescriptionSets.resize(Count);

	VK_CHECK(vkAllocateDescriptorSets(*GRHI->GetDevice(), &AllocInfo, DescriptionSets.data()));

}
void
SMaterialInterface::CreatePipeline(FRenderPass * RenderPass)
{
	GraphicPipeline = TSharedPtr<FGraphicsPipeline>(new FGraphicsPipeline(
		{Shader.get(),RenderPass} ));
}
void
SMaterialInterface::InitMaterialParameters()
{
	if(Shader == nullptr) return ;
	Shader->GenerateDefaultMaterialParams(MaterialParameters,this );
}
void
SMaterialInterface::Initialize(VkDescriptorPool Pool, FRenderPass *RenderPass)
{
	if(Shader)
	{
		Shader->AddUsedMaterial(this);
	}
	CreateDescriptionSets(Pool);
	CreatePipeline(RenderPass);
	InitMaterialParameters();
	if(bNeedViewData)
	{
		OnSetupViewData();
		OnSetupLightData();
	}
}

void SMaterialInterface::OnPreRecordCommandBuffer(FPrimitiveRenderData* InRenderData, uint32 CurrentFrame,
	FPreRecordBufferContext& Context)
{
	if(bDirty)
	{
		CreatePipeline(Context.RenderPass);
		bDirty = false;
	}
	OnSetupPrimitiveData(InRenderData,CurrentFrame);
}

void
SMaterialInterface::OnRecordCommandBuffer(VkCommandBuffer CommandBuffer,uint32 CurrentFrame)
{

	vkCmdBindDescriptorSets(CommandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,
							GraphicPipeline->GetLayout(),0,1,&DescriptionSets[CurrentFrame],
							0,nullptr );
	vkCmdBindPipeline(CommandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,
					  GraphicPipeline->GetHandle()
					  );

}
void
SMaterialInterface::SetTexture(uint32 Binding, TSharedPtr<FTexture2D> InTexture)
{
	MaterialParameters.SetTexture(Binding,InTexture);
	for(auto & DescriptionSet :DescriptionSets)
	{
		MaterialParameters.BindParametersToDescriptorSet(DescriptionSet);
	}
}

void SMaterialInterface::SetTextureCube(uint32 Binding, TSharedPtr<FTextureCubeRHI> CubeTexture)
{
	MaterialParameters.SetTexture(Binding,CubeTexture);
	for(auto & DescriptionSet :DescriptionSets)
	{
		MaterialParameters.BindParametersToDescriptorSet(DescriptionSet);
	}
}

void SMaterialInterface::SetTexture(const FString& Name, TSharedPtr<FTexture2D> InTexture)
{
	MaterialParameters.SetTexture(Name,InTexture);
	for(auto & DescriptionSet :DescriptionSets)
	{
		MaterialParameters.BindParametersToDescriptorSet(DescriptionSet);
	}
}

void SMaterialInterface::SetSampler(const FString &Name, TSharedPtr<FTextureBase> InTexture)
{
	MaterialParameters.SetTextureSampler(Name,InTexture);
	for(auto & DescriptionSet :DescriptionSets)
	{
		MaterialParameters.BindParametersToDescriptorSet(DescriptionSet);
	}

}

void SMaterialInterface::SetTextureCube(const FString &Name, TSharedPtr<FTextureCubeRHI> CubeTexture)
{
    MaterialParameters.SetTexture(Name,CubeTexture);
    for(auto & DescriptionSet :DescriptionSets)
    {
        MaterialParameters.BindParametersToDescriptorSet(DescriptionSet);
    }
}

bool SMaterialInterface::SetVector(const FString &Name, const FVector4 &InVector)
{
	auto Result = MaterialParameters.SetVector(Name,InVector);
	/*for(auto & DescriptionSet :DescriptionSets)
	{
		MaterialParameters.BindParametersToDescriptorSet(DescriptionSet);
	}*/
	return Result;
}

bool SMaterialInterface::SetInt(const FString &Name, int32 IntValue)
{
    auto Result = MaterialParameters.SetInt(Name,IntValue);
    return Result;
}

bool SMaterialInterface::SetScalar(const FString &Name, float InValue)
{
    auto Result = MaterialParameters.SetScalar(Name,InValue);
    return Result;
}

void SMaterialInterface::OnSetupViewData()
{
	auto SceneView = GEngine->GetRenderer()->GetSceneView();
	if(SceneView == nullptr) return;

	for(uint32 Idx = 0 ; Idx < DescriptionSets.size() ; ++ Idx)
	{
		VkDescriptorBufferInfo BufferInfo =
			{
			.buffer = SceneView->GetUniformBuffer(Idx)->GetBuffer(),
			.offset = 0,
			.range = sizeof(FViewDataShape)
			};
		VkWriteDescriptorSet DescriptorWrite{};
		DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		DescriptorWrite.dstSet = DescriptionSets[Idx];
		DescriptorWrite.dstBinding = 0;
		DescriptorWrite.dstArrayElement = 0;
		DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		DescriptorWrite.descriptorCount = 1;
		DescriptorWrite.pBufferInfo = &BufferInfo;

		vkUpdateDescriptorSets(*GRHI->GetDevice(),
		                       1,&DescriptorWrite ,0, nullptr);

	}

}

void SMaterialInterface::OnSetupLightData()
{

	auto LightData = GEngine->GetRenderer()->GetLightData();
	if(LightData == nullptr) return;

	for(uint32 Idx = 0 ; Idx < DescriptionSets.size() ; ++ Idx)
	{
		VkDescriptorBufferInfo BufferInfo =
			{
			.buffer = LightData->GetUniformBuffer(Idx)->GetBuffer(),
			.offset = 0,
			.range = sizeof(FLightRenderDataShape)
			};
		VkWriteDescriptorSet DescriptorWrite{};
		DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		DescriptorWrite.dstSet = DescriptionSets[Idx];
		DescriptorWrite.dstBinding = 2;
		DescriptorWrite.dstArrayElement = 0;
		DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		DescriptorWrite.descriptorCount = 1;
		DescriptorWrite.pBufferInfo = &BufferInfo;

		vkUpdateDescriptorSets(*GRHI->GetDevice(),
		                       1,&DescriptorWrite ,0, nullptr);

	}
}

void SMaterialInterface::OnSetupPrimitiveData(FPrimitiveRenderData* InRenderData, uint32 CurrentFrame)
{
	if(InRenderData == nullptr) return ;

	//for(uint32 Idx = 0 ; Idx < DescriptionSets.size() ; ++ Idx)
	{
		VkDescriptorBufferInfo BufferInfo =
			{
			.buffer = InRenderData->GetUniformBuffer(CurrentFrame)->GetBuffer(),
			.offset = 0,
			.range = sizeof(FPerPrimitiveRenderDataShape)
			};
		VkWriteDescriptorSet DescriptorWrite{};
		DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		DescriptorWrite.dstSet = DescriptionSets[CurrentFrame];
		DescriptorWrite.dstBinding = 1;
		DescriptorWrite.dstArrayElement = 0;
		DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		DescriptorWrite.descriptorCount = 1;
		DescriptorWrite.pBufferInfo = &BufferInfo;

		vkUpdateDescriptorSets(*GRHI->GetDevice(),
		                       1,&DescriptorWrite ,0, nullptr);

	}
}

void SMaterialInterface::SyncToCommandBuffer(VkCommandBuffer CommandBuffer)
{
    MaterialParameters.OnSyncToCommandBuffer(CommandBuffer);
}




