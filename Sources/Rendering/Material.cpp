//
// Created by 郑文凯 on 2024/10/2.
//

#include "Material.h"
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
	Shader->GenerateDefaultMaterialParams(MaterialParameters);
}
void
SMaterialInterface::Initialize(VkDescriptorPool Pool, FRenderPass *RenderPass)
{
	CreateDescriptionSets(Pool);
	CreatePipeline(RenderPass);
	InitMaterialParameters();
	OnSetupViewData();
}
void
SMaterialInterface::OnRecordCommandBuffer(VkCommandBuffer CommandBuffer,uint32 CurrentFrame)
{
	vkCmdBindPipeline(CommandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,
					  GraphicPipeline->GetHandle()
					  );
	vkCmdBindDescriptorSets(CommandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,
							GraphicPipeline->GetLayout(),0,1,&DescriptionSets[CurrentFrame],
							0,nullptr );

}
void
SMaterialInterface::SetTexture(uint32 Binding, TSharedPtr<FTexture> InTexture)
{
	MaterialParameters.SetTexture(Binding,InTexture);
	for(auto & DescriptionSet :DescriptionSets)
	{
		MaterialParameters.BindParametersToDescriptorSet(DescriptionSet);
	}
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

	auto PrimitiveData = GEngine->GetRenderer()->GetPrimitiveData();
	if(PrimitiveData == nullptr) return ;

	for(uint32 Idx = 0 ; Idx < DescriptionSets.size() ; ++ Idx)
	{
		VkDescriptorBufferInfo BufferInfo =
			{
			.buffer = PrimitiveData->GetUniformBuffer(Idx)->GetBuffer(),
			.offset = 0,
			.range = sizeof(FPerPrimitiveRenderDataShape)
			};
		VkWriteDescriptorSet DescriptorWrite{};
		DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		DescriptorWrite.dstSet = DescriptionSets[Idx];
		DescriptorWrite.dstBinding = 1;
		DescriptorWrite.dstArrayElement = 0;
		DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		DescriptorWrite.descriptorCount = 1;
		DescriptorWrite.pBufferInfo = &BufferInfo;

		vkUpdateDescriptorSets(*GRHI->GetDevice(),
		                       1,&DescriptorWrite ,0, nullptr);

	}



}
