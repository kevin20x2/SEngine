//
// Created by kevin on 2024/12/7.
//

#include "PostProcessManager.h"

#include "Material.h"
#include "Renderer.h"
#include "Shader.h"
#include "volk.h"
#include "CoreObjects/SObject.h"
#include "Maths/Math.h"
#include "Systems/ShaderManager/ShaderManager.h"


void FPostProcessManager::InitRenderResource()
{

    TArray <FVector> Positions = {
        FVector(-1.0f,-1,0.5),
        FVector(4,-1,0.5),
        FVector(-1,4,0.5)
    };
    TArray <FVector2> Uv =
    {
        FVector2(0,0),
        FVector2(2,0),
        FVector2(0,2)
    };

    const uint32 Stride = sizeof(FVector) + sizeof(FVector2);
    uint32 BufferSize = Positions.size() * Stride;
    TArray<char > Buffer;
    Buffer.resize(BufferSize);
    char * pBuffer = Buffer.data();

    for(uint32 Idx = 0 ; Idx < Positions.size(); ++ Idx)
    {
        memcpy(pBuffer,&Positions[Idx],sizeof(FVector));
        pBuffer  += sizeof(FVector);
        memcpy(pBuffer,&Uv[Idx],sizeof(FVector2));
        pBuffer +=sizeof(FVector2);
    }

    TriVB = TSharedPtr<FVertexBuffer>(new FVertexBuffer({
        BufferSize,(float * ) Buffer.data()
    }));

    TArray <uint16> Indexes = {0,1,2};

    TriIB.reset(new FIndexBuffer({
        uint32(Indexes.size() * sizeof(uint16)),
        Indexes.data()
    }));

    auto Shader = SShaderManager::GetShaderFromName("Blit");
    BlitMaterial = SNew<SMaterialInterface>(Shader->AsShared());
    auto Renderer = GEngine->GetModuleByClass<SRenderer>();
    BlitMaterial->SetbNeedViewData(false);
    BlitMaterial->Initialize(Renderer->GetDescriptorPool()->Pool,Renderer->GetSwapChainRenderPass());


}

void FPostProcessManager::BeginPostProcess(uint32 FrameIdx,
    VkCommandBuffer CommandBuffer ,FRenderTargetGroup* InBaseGroup, FRenderTargetGroup* SwapChainGroup)
{

    BlitMaterial->OnRecordCommandBuffer(CommandBuffer,FrameIdx);

    VkBuffer VertexBuffers[] = {TriVB->GetHandle()};
    VkDeviceSize Offsets[] = {0};

    vkCmdBindVertexBuffers(CommandBuffer,0,1,VertexBuffers,Offsets);
    vkCmdBindIndexBuffer( CommandBuffer,TriIB->GetHandle(),0,VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(CommandBuffer,3,1,0,0,0);

}

void FPostProcessManager::OnPreRecordCommandBuffer(uint32 ImageIndex, FRenderTargetGroup* InBaseGroup,
    FRenderTargetGroup* SwapChainGroup)
{
    BlitMaterial->SetTexture("texColor", InBaseGroup->GetRenderTexture(ImageIndex));
}

