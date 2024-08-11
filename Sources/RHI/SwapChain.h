//
// Created by kevin on 2024/8/11.
//

#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H
#include <vulkan/vulkan_core.h>

#include "Core/BaseTypes.h"


class FSwapChain
{
public:
    static FSwapChain * CreateSwapChain(VkExtent2D DisplaySize);

    virtual VkFormat GetFormat() = 0;
};



#endif //SWAPCHAIN_H
