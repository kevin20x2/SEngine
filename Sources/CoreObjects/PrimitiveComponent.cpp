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

}

void SPrimitiveComponent::OnRecordCommandBuffer(VkCommandBuffer CommandBuffer,uint32 CurrentFrame)
{
	if(Material)
	{
		Material->OnRecordCommandBuffer(CommandBuffer,CurrentFrame);
	}
	auto PrimitiveData = GEngine->GetRenderer()->GetPrimitiveData();
	PrimitiveData->UpdateModelMatrix( GetWorldTransform().ToMatrix() );
	PrimitiveData->SyncData(CurrentFrame);

    VkBuffer VertexBuffers[] = {VB->GetHandle()};
    VkDeviceSize Offsets[] = {0};

    vkCmdBindVertexBuffers(CommandBuffer,0,1,VertexBuffers,Offsets);
    vkCmdBindIndexBuffer(CommandBuffer,IB->GetHandle(),0,VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(CommandBuffer, Mesh->Indexes.size(),1,0,0,0);

}

void SPrimitiveComponent::OnRegister()
{
	SCoreComponentBase::OnRegister();
	GEngine->GetRenderer()->OnAddPrimitive(this);
}
void
SPrimitiveComponent::OnUnRegister()
{
	SCoreComponentBase::OnUnRegister();
	GEngine->GetRenderer()->OnRemovePrimitive(this);
}

