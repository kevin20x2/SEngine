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


    virtual VkSwapchainKHR * GetSwapChain() = 0;

    virtual VkFormat GetFormat() = 0;

    virtual VkImageView * GetView(int32 Idx) = 0 ;

    virtual VkExtent2D GetExtent() = 0;

    virtual void CleanUp() = 0;
};



#endif //SWAPCHAIN_H
