//
// Created by kevin on 2024/8/17.
//

#include "PrimitiveComponent.h"

#include "volk.h"
#include "Engine.h"

void SPrimitiveComponent::CreateRHIResource()
{
    if(Mesh == nullptr) return ;

    VB = Mesh->CreateVertexBuffer();

    IB.reset(new FIndexBuffer({uint32 (Mesh->Indexes.size() * sizeof(uint16)),
        (uint16*)Mesh->Indexes.data()}));

	RenderData = SNew<FPrimitiveRenderData>();

}

void SPrimitiveComponent::OnRecordCommandBuffer(VkCommandBuffer CommandBuffer,uint32 CurrentFrame)
{
	RenderData->UpdateModelMatrix( GetWorldTransform().ToMatrix() );
	RenderData->SyncData(CurrentFrame);
	if(Material)
	{
		Material->OnRecordCommandBuffer(CommandBuffer,CurrentFrame);
		Material->OnSetupPrimitiveData(RenderData.get(),CurrentFrame);
	}

    VkBuffer VertexBuffers[] = {VB->GetHandle()};
    VkDeviceSize Offsets[] = {0};

    vkCmdBindVertexBuffers(CommandBuffer,0,1,VertexBuffers,Offsets);
    vkCmdBindIndexBuffer(CommandBuffer,IB->GetHandle(),0,VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(CommandBuffer, (uint32)Mesh->Indexes.size(),1,0,0,0);

}

void SPrimitiveComponent::OnRegister()
{
	SCoreComponentBase::OnRegister();
	if(VB == nullptr)
	{
		CreateRHIResource();
	}
	GEngine->GetRenderer()->OnAddPrimitive(this);
}
void
SPrimitiveComponent::OnUnRegister()
{
	SCoreComponentBase::OnUnRegister();
	GEngine->GetRenderer()->OnRemovePrimitive(this);
}

