//
// Created by kevin on 2024/8/11.
//

#ifndef RHIUTILS_H
#define RHIUTILS_H
#include <string>
#include <vulkan/vulkan_core.h>

#include "RHI.h"
#include "Shader.h"


class FRHIUtils
{
    friend class FRHIImp ;
public:

    static bool IsDeivceSuitable(VkPhysicalDevice Device,VkSurfaceKHR Surface );

    static FQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice Device,VkSurfaceKHR Surface);

    static FSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice Device,VkSurfaceKHR Surface);

    static uint32 FindMemoryType(uint32 TypeFilter , VkMemoryPropertyFlags Properties);

    static VkShaderModule LoadHlslShaderByFilePath(const std::string & FilePath, VkShaderStageFlagBits Stage, TArray <FDescriptorSetLayoutInfo> & Infos );

	static void CreateBuffer(VkBufferUsageFlagBits Usage,uint32 BufferSize,
							 VkFlags PropertyFlags,VkBuffer & Buffer, VkDeviceMemory & DeviceMemory);


	static void CreateImage(uint32 Height, uint32 Width,VkFormat Format,VkImageTiling Tiling,
							VkImageUsageFlags UsageFlags,VkFlags PropertyFlags,VkImage& Image,VkDeviceMemory & DeviceMemory );

	static VkCommandBuffer BeginOneTimeCommandBuffer();

	static void EndOneTimeCommandBuffer(VkCommandBuffer CommandBuffer);



	template <typename RetType ,typename... Args>
	static void OneTimeCommand(TFunction<RetType(VkCommandBuffer,Args...)> && Function, Args... InArgs)
	{
		auto CommandBuffer = BeginOneTimeCommandBuffer();
		Function(CommandBuffer,InArgs...);
		EndOneTimeCommandBuffer(CommandBuffer);
	}

	static void OneTimeCommand(std::function<void(VkCommandBuffer)> && Function)
	{
		auto CommandBuffer = BeginOneTimeCommandBuffer();
		Function(CommandBuffer);
		EndOneTimeCommandBuffer(CommandBuffer);
	}
	static void TransitionImageLayout(VkImage Image,VkFormat Format, VkImageLayout SrcLayout ,VkImageLayout DstLayout  );

	static void CopyBufferToImage(VkImage Image , VkBuffer Buffer, uint32 Height, uint32 Width);
};




#endif //RHIUTILS_H
