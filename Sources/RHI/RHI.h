//
// Created by kevin on 2024/8/11.
//

#pragma once
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Core/BaseTypes.h"


#define VK_CHECK(x) \
	do {			\
VkResult  error = x;\
	if (error)\
	{				\
				abort();\
	}				\
				\
	}while(0)\


class FRHI
{
public:
    virtual VkDevice * GetDevice() = 0;
    virtual VkInstance * GetInstance() = 0 ;
    virtual VkPhysicalDevice * GetPhysicalDevice() = 0;
    virtual VkSurfaceKHR * GetCurSurface() = 0;

    virtual uint32 GetMaxFrameInFlight() = 0;

};


struct FQueueFamilyIndices
{
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentFamily;

    bool IsComplete() const
    {
        return GraphicsFamily.has_value() && PresentFamily.has_value();
    }
};


struct FSwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR Capabilites;

    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};


bool InitRHI(class FWindow * InWindow);

extern FRHI * GRHI ;
