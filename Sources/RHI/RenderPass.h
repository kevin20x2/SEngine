//
// Created by kevin on 2024/8/11.
//

#ifndef RENDERPASS_H
#define RENDERPASS_H
#include <vulkan/vulkan_core.h>

#include "SwapChain.h"


class FRenderPass
{

public:
    static FRenderPass * Create(VkFormat InFormat,VkImageLayout ImageLayout, uint32 ColorCount = 1,bool NeedDepth = true);

    virtual ~FRenderPass();

    VkRenderPass RenderPass;

    uint32 ColorAttachmentNum = 1;
};

#endif //RENDERPASS_H
