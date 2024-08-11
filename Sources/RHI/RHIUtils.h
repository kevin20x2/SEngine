//
// Created by kevin on 2024/8/11.
//

#ifndef RHIUTILS_H
#define RHIUTILS_H
#include <vulkan/vulkan_core.h>

#include "RHI.h"


class FRHIUtils
{
    friend class FRHIImp ;

    static bool IsDeivceSuitable(VkPhysicalDevice Device,VkSurfaceKHR Surface );

    static FQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice Device,VkSurfaceKHR Surface);

    static FSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice Device,VkSurfaceKHR Surface);

};



#endif //RHIUTILS_H
