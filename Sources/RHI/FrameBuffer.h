//
// Created by kevin on 2024/8/11.
//

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <vulkan/vulkan_core.h>

#include "RenderPass.h"
#include "Core/BaseTypes.h"


class FSwapChain;
class FDepthTexture;

class FFrameBuffer
{
public:

    FFrameBuffer(int32 ViewIdx ,
				 FRenderPass * InRenderPass,FSwapChain * InSwapChain, TArray < TSharedPtr <FDepthTexture >>  InDepthTexList);

    FFrameBuffer(FRenderPass* InRenderPass,
        uint32 Width , uint32 Height ,VkImageView ColorView,VkImageView DepthView);

    FFrameBuffer(FRenderPass * InRenderPass,
        uint32 Width ,uint32 Height,const TArray <VkImageView> & ImageViews
        );

    virtual ~FFrameBuffer();
    void Destroy();

    VkFramebuffer FrameBuffer;
};



#endif //FRAMEBUFFER_H
