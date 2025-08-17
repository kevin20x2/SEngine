//
// Created by kevin on 2025/8/16.
//

#include "RenderTargetCube.h"
#include "RHI/RHI.h"
#include "volk.h"


void FRenderTargetCube::Initialize(uint32 InWidth, uint32 InHeight,uint8 * InData)
{
    Width = InWidth;
    Height = InHeight;
    FTextureCubeCreateParams CubeCreateParams =
    {
            .Width = Width,
            .Height = Height,
            .Format = VK_FORMAT_R8G8B8A8_SRGB,
            .Data = InData,
            .UsageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |VK_IMAGE_USAGE_SAMPLED_BIT,
            .bCreateMips = true
    };

    CubeTexture = FTextureCubeRHI::Create(CubeCreateParams);

    RenderPass = TSharedPtr <FRenderPass> (  FRenderPass::Create(
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,1,false ));

    uint32 MipLevels = CubeTexture->GetMipLevels();
    FrameBuffers.resize(MipLevels * 6 );

    auto Device = *GRHI->GetDevice();
    TArray <VkImageView> Attachments;
    CubeRT2DViews.empty();
    uint32 CurWidth = Width;
    uint32 CurHeight = Height;
    for(uint32 Mip = 0 ; Mip <MipLevels; ++Mip)
    {
        auto & Views = CubeRT2DViews.emplace_back();
        for(uint32 Face = 0 ; Face < 6; ++ Face)
        {
            VkImageViewCreateInfo CreateInfo = {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .image = CubeTexture->GetImage(),
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = VK_FORMAT_R8G8B8A8_SRGB,
                    .subresourceRange = {
                            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                            .baseMipLevel = Mip,
                            .levelCount = 1,
                            .baseArrayLayer = Face,
                            .layerCount = 1
                    }
            };
            VkImageView ImageView;

            vkCreateImageView(Device,&CreateInfo,nullptr,&ImageView);
            Views.Add(ImageView);

            FrameBuffers[Mip * 6 + Face] = TSharedPtr<FFrameBuffer>(
                    new FFrameBuffer(RenderPass.get(),CurWidth,CurHeight,{ImageView})
                    );
        }
        CurWidth = max(CurWidth / 2 , 1);
        CurHeight = max(CurHeight / 2 , 1);
    }
}

TSharedPtr<FRenderTargetCube> FRenderTargetCube::Create(uint32 Width, uint32 Height, uint8 *InData)
{
    auto Cube = TSharedPtr<FRenderTargetCube>(new FRenderTargetCube());
    Cube->Initialize(Width,Height,InData);
    return Cube;
}

void FRenderTargetCube::BeginRenderTargetGroup(VkCommandBuffer CommandBuffer,int32 MipIdx, int32 FaceIdx)
{
    auto MipWidth = (Width >> MipIdx);
    auto MipHeight= (Height >> MipIdx);

    VkExtent2D Extent = {.width = MipWidth, .height = MipHeight};
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass->RenderPass;
    renderPassInfo.framebuffer = FrameBuffers[MipIdx * 6 + FaceIdx]->FrameBuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = Extent;


    VkViewport viewport{};
    viewport.width = (float)Extent.width;
    viewport.height = (float)Extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.extent = Extent;

    TArray<VkClearValue> ClearColor = { {.color = {0,0,0,1}}};

    renderPassInfo.clearValueCount = ClearColor.size();
    renderPassInfo.pClearValues = ClearColor.data();
    vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdSetScissor(CommandBuffer, 0, 1, &scissor);
    vkCmdSetViewport(CommandBuffer, 0, 1, &viewport);

}

void FRenderTargetCube::EndRenderTargetGroup(VkCommandBuffer CommandBuffer)
{
    vkCmdEndRenderPass(CommandBuffer);
}
