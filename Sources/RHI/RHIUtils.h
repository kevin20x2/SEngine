//
// Created by kevin on 2024/8/11.
//

#ifndef RHIUTILS_H
#define RHIUTILS_H
#include <string>
#include <vulkan/vulkan_core.h>

#include "RHI.h"
#include "ShaderProgram.h"
#include "spirv_reflect.h"


class FRHIUtils
{
    friend class FRHIImp ;
public:

    static bool IsDeivceSuitable(VkPhysicalDevice Device,VkSurfaceKHR Surface, uint32 & OutIndex );

    static FQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice Device,VkSurfaceKHR Surface);

    static FSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice Device,VkSurfaceKHR Surface);

    static uint32 FindMemoryType(uint32 TypeFilter , VkMemoryPropertyFlags Properties);

    static VkShaderModule LoadHlslShaderByFilePath(const std::string & FilePath,
												   VkShaderStageFlagBits Stage,
												   TArray <FDescriptorSetLayoutInfo> & Infos,
												   FVertexInputInfo & OutVertexInputInfo
												   );
	static FString GetShaderEntryPointByShaderStage(VkShaderStageFlagBits State);


	static FVertexInputInfo GenerateVertexInputStateCreateInfo(TArray <SpvReflectInterfaceVariable *> InputVars);

	static void CreateBuffer(VkBufferUsageFlags Usage,uint32 BufferSize,
							 VkFlags PropertyFlags,VkBuffer & Buffer, VkDeviceMemory & DeviceMemory);


	static void CreateImage(uint32 Height, uint32 Width,VkFormat Format,VkImageTiling Tiling,
							VkImageUsageFlags UsageFlags,VkFlags PropertyFlags,VkImage& Image,VkDeviceMemory & DeviceMemory,int32 LayerNum = 1 );

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
	static uint32_t FormatSize(VkFormat Format);

	static void TransitionImageLayout(
		VkImage Image,
		VkFormat Format,
		VkImageLayout SrcLayout , VkImageLayout DstLayout,
		uint32 BaseMipLevel = 0 ,
		uint32 MipLevelCount = 1,
		uint32 BaseLayer = 0,
		uint32 LayerCount = 1);


	static void TransitionImageLayoutWithCommandBuffer(
		VkCommandBuffer CommandBuffer,
		VkImage Image,
		VkFormat Format,
		VkImageLayout SrcLayout , VkImageLayout DstLayout,
		uint32 BaseMipLevel = 0 ,
		uint32 MipLevelCount = 1,
		uint32 BaseLayer = 0,
		uint32 LayerCount = 1);

	static void CopyBufferToImage(VkImage Image , VkBuffer Buffer, uint32 Height, uint32 Width,int32 LayerCount = 1);

	static void CreateImageView(VkImage Image,VkFormat Format, VkImageAspectFlags AspectFlags,VkImageView & ImageView);

	static VkFormat FindSupportFormat(const TArray <VkFormat> & Formats,VkImageTiling ImageTiling, VkFormatFeatureFlags FeatureFlags);

	static VkFormat FindDepthFormat();

	static bool
	HasStencilComponent(VkFormat format);
};




#endif //RHIUTILS_H
