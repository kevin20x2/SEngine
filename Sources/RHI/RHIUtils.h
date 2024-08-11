//
// Created by kevin on 2024/8/11.
//

#ifndef RHIUTILS_H
#define RHIUTILS_H
#include <string>
#include <vulkan/vulkan_core.h>

#include "RHI.h"


class FRHIUtils
{
    friend class FRHIImp ;
public:

    static bool IsDeivceSuitable(VkPhysicalDevice Device,VkSurfaceKHR Surface );

    static FQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice Device,VkSurfaceKHR Surface);

    static FSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice Device,VkSurfaceKHR Surface);

    static uint32 FindMemoryType(uint32 TypeFilter , VkMemoryPropertyFlags Properties);

    static VkShaderModule LoadHlslShaderByFilePath(const std::string & FilePath, VkShaderStageFlagBits Stage);

};



#endif //RHIUTILS_H
