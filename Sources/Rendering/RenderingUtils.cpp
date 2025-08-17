//
// Created by kevin on 2025/8/17.
//

#include "RenderingUtils.h"
#include "RHI/IndexBuffer.h"
#include "volk.h"
#include "CoreObjects/Engine.h"
#include "Rendering/Renderer.h"

static TSharedPtr <FVertexBuffer> ScreenTriangleVB ;
static TSharedPtr <FIndexBuffer> ScreenTriangleIB ;


void InitScreenRHI()
{
    TArray <FVector> Positions = {
            FVector(-1.0f,-1,0.5),
            FVector(1,-1,0.5),
            FVector(-1,1,0.5),
            FVector(1,1,0.5)
    };
    TArray <FVector2> Uv =
            {
                    FVector2(0,0),
                    FVector2(1,0),
                    FVector2(0,1),
                    FVector2(1,1)
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

    ScreenTriangleVB = TSharedPtr<FVertexBuffer>(new FVertexBuffer({
                                                                BufferSize,(float * ) Buffer.data()
                                                        }));

    TArray <uint16> Indexes = {0,1,2,2,1,3};

    ScreenTriangleIB.reset(new FIndexBuffer({
                                         uint32(Indexes.size() * sizeof(uint16)),
                                         Indexes.data()
                                 }));


}


void FRenderingUtils::DrawScreenTriangle(VkCommandBuffer CommandBuffer,
                                         SMaterialInterface *InMaterial)
{
    if(ScreenTriangleIB == nullptr || ScreenTriangleVB == nullptr)
    {
        InitScreenRHI();
    }

    auto CurrentFrame = GEngine->GetRenderer()->GetFrameIndex();

    InMaterial->OnRecordCommandBuffer(CommandBuffer,CurrentFrame);

    VkBuffer VertexBuffers[] = {ScreenTriangleVB->GetHandle()};
    VkDeviceSize Offsets[] = {0};

    vkCmdBindVertexBuffers(CommandBuffer,0,1,VertexBuffers,Offsets);
    vkCmdBindIndexBuffer( CommandBuffer,ScreenTriangleIB->GetHandle(),0,VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(CommandBuffer,6,1,0,0,0);

}
