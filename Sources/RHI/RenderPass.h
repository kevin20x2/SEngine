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
    static FRenderPass * Create(FSwapChain * SwapChain );

    VkRenderPass RenderPass;
};

#endif //RENDERPASS_H
