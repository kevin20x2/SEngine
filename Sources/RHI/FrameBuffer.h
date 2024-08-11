//
// Created by kevin on 2024/8/11.
//

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <vulkan/vulkan_core.h>

#include "RenderPass.h"
#include "Core/BaseTypes.h"


class FSwapChain;

class FFrameBuffer
{
public:

    FFrameBuffer(int32 ViewIdx ,FRenderPass * InRenderPass,FSwapChain * InSwapChain);

    VkFramebuffer FrameBuffer;
};



#endif //FRAMEBUFFER_H
