//
// Created by kevin on 2024/8/11.
//

#include "RHIUtils.h"

#include <vector>

#include "volk.h"

bool FRHIUtils::IsDeivceSuitable(VkPhysicalDevice Device, VkSurfaceKHR Surface)
{
	auto Indice = FindQueueFamilies(Device,Surface);
	bool SwapChainAdequate = false;

	auto SwapChainSupportDetail = QuerySwapChainSupport(Device,Surface);
	SwapChainAdequate = !SwapChainSupportDetail.Formats.empty() &&
						!SwapChainSupportDetail.PresentModes.empty();


	return Indice.IsComplete() && SwapChainAdequate;
}

FQueueFamilyIndices FRHIUtils::FindQueueFamilies(VkPhysicalDevice Device, VkSurfaceKHR Surface)
{

	FQueueFamilyIndices Result;

	uint32_t QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount,nullptr);

	std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilies.data());

	int i = 0 ;

	for(const auto &QueueFamily : QueueFamilies)
	{
		if(QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			Result.GraphicsFamily = i ;
		}

		VkBool32 PresentSupport =false;
		vkGetPhysicalDeviceSurfaceSupportKHR(Device,i,Surface,&PresentSupport);
		if(PresentSupport)
		{
			Result.PresentFamily = i ;
		}

		if(Result.IsComplete())
		{
			break ;
		}
		i++ ;
	}

	return Result;
}

FSwapChainSupportDetails FRHIUtils::QuerySwapChainSupport(VkPhysicalDevice Device, VkSurfaceKHR Surface)
{

	FSwapChainSupportDetails Result;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device,Surface,&Result.Capabilites );

	uint32_t FormatCount ;

	vkGetPhysicalDeviceSurfaceFormatsKHR(Device,Surface, &FormatCount, nullptr);

	if(FormatCount != 0)
	{
		Result.Formats.resize(FormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(Device,Surface,&FormatCount, Result.Formats.data());
		printf("physicalDevice Format Count : %d", FormatCount);
	}

	uint32_t PresentModeCount = 0 ;
	vkGetPhysicalDeviceSurfacePresentModesKHR(Device,Surface,&PresentModeCount,nullptr);

	if(PresentModeCount != 0)
	{
		Result.PresentModes.resize(PresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(Device,Surface,&PresentModeCount,
												  Result.PresentModes.data()
												  );
	}
	return Result;
}
