//
// Created by kevin on 2024/8/11.
//

#include "RHIUtils.h"

#include <fstream>
#include <ResourceLimits.h>
#include <vector>

#include "volk.h"
#include "glslang/Public/ShaderLang.h"
#include "SPIRV/GlslangToSpv.h"
#include "SPIRV/Logger.h"
#include "CoreObjects/Engine.h"
#include "spirv_reflect.h"

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

uint32 FRHIUtils::FindMemoryType(uint32 TypeFilter, VkMemoryPropertyFlags Properties)
{
	VkPhysicalDeviceMemoryProperties MemProperties;
	vkGetPhysicalDeviceMemoryProperties(* GRHI->GetPhysicalDevice(), &MemProperties);

	for (uint32_t i = 0; i < MemProperties.memoryTypeCount; ++i)
	{
		if ((TypeFilter & (1 << i)) &&
			((MemProperties.memoryTypes[i].propertyFlags & Properties) == Properties))
		{
			return i;
		}
	}
	return -1;
}

VkShaderModule FRHIUtils::LoadHlslShaderByFilePath(const std::string& FilePath, VkShaderStageFlagBits Stage,TArray <FDescriptorSetLayoutInfo> & LayoutInfos)
{

	std::vector <uint32_t> Spirv;
	std::string InfoLog;

	glslang::InitializeProcess();

	auto Messages = static_cast<EShMessages>(EShMsgReadHlsl | EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);

	EShLanguage Language{};

	switch (Stage)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		Language = EShLangVertex;
		break;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		Language = EShLangFragment;
		break;
	default:
		break;
	}




	//Shader.setStringsWithLengths()
	std::ifstream ifs(FilePath);

	if (!ifs.is_open())
	{
		throw std::runtime_error("Source File Not Exist");

	}

	std::string Source((std::istreambuf_iterator<char>(ifs)),std::istreambuf_iterator<char>( ) );

	const char* ShaderSource = reinterpret_cast <const char*>(Source.data());

	glslang::TShader Shader(Language);
	Shader.setStringsWithLengths(&ShaderSource,nullptr ,1);
	Shader.setEnvInput(glslang::EShSourceHlsl, Language, glslang::EShClientVulkan, 1);
	Shader.setEntryPoint("main");
	Shader.setSourceEntryPoint("main");
	Shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);
	Shader.setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_0);

	if (!Shader.parse(&glslang::DefaultTBuiltInResource, 100, false, Messages))
	{
		throw std::runtime_error("Fail to parse HLSL Shader");
	}

	glslang::TProgram Program;

	Program.addShader(&Shader);

	if (!Program.link(Messages))
	{
		printf("Failed to parse HLSL Shader ,Error %s \ n  %s", Shader.getInfoDebugLog(), Shader.getInfoDebugLog());
		throw std::runtime_error("Fail to parse HLSL Shader");
	}


	if (Shader.getInfoLog())
	{
		InfoLog += std::string(Shader.getInfoLog()) + "\n" + std::string(Shader.getInfoDebugLog()) + "\n";
	}

	if (Program.getInfoLog())
	{
		InfoLog += std::string(Program.getInfoLog()) + "\n" + std::string(Program.getInfoDebugLog()) + "\n";
	}

	glslang::TIntermediate* Intermediate = Program.getIntermediate(Language);

	if (!Intermediate)
	{
		throw std::runtime_error("Fail to Compile HLSL Shader");
	}

	spv::SpvBuildLogger Logger;

	glslang::GlslangToSpv(*Intermediate, Spirv, &Logger);

	InfoLog += Logger.getAllMessages() + "\n";

	glslang::FinalizeProcess();

	{ // reflect
		SpvReflectShaderModule ReflectShaderModule = {};
		auto Result = spvReflectCreateShaderModule(Spirv.size() * sizeof(uint32), Spirv.data(), &ReflectShaderModule);
		assert(Result == SPV_REFLECT_RESULT_SUCCESS);

		uint32 DescriptorCount = 0;
		Result = spvReflectEnumerateDescriptorSets(&ReflectShaderModule, &DescriptorCount, nullptr);
		assert(Result == SPV_REFLECT_RESULT_SUCCESS);

		TArray<SpvReflectDescriptorSet *> Sets(DescriptorCount);

		Result = spvReflectEnumerateDescriptorSets(&ReflectShaderModule, &DescriptorCount, Sets.data());

		LayoutInfos.resize(DescriptorCount);

		for(uint32 i = 0; i < Sets.size(); ++ i )
		{
			const auto & ReflectSet =  *Sets[i];
			auto & LayoutInfo = LayoutInfos[i];
			LayoutInfo.Bindings.resize(ReflectSet.binding_count);
			for(uint32 BindingIdx = 0 ; BindingIdx < ReflectSet.binding_count ; ++ BindingIdx )
			{
				const auto & RefBinding = *ReflectSet.bindings[BindingIdx];
				VkDescriptorSetLayoutBinding & Binding = LayoutInfo.Bindings[BindingIdx];
				Binding.binding = RefBinding.binding;
				Binding.descriptorType = static_cast<VkDescriptorType>(RefBinding.descriptor_type);
				Binding.descriptorCount = 1;

				for(uint32 Dim = 0 ; Dim < RefBinding.array.dims_count ; ++ Dim)
				{
					Binding.descriptorCount += RefBinding.array.dims[Dim];
				}
				Binding.stageFlags = static_cast<VkShaderStageFlagBits>(ReflectShaderModule.shader_stage);
			}
			LayoutInfo.SetNumber = ReflectSet.set;
			LayoutInfo.CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			LayoutInfo.CreateInfo.bindingCount = ReflectSet.binding_count;
			LayoutInfo.CreateInfo.pBindings = LayoutInfo.Bindings.data();
		}
		spvReflectDestroyShaderModule(&ReflectShaderModule);
	}

	VkShaderModule ShaderModule;
	VkShaderModuleCreateInfo ModuleCreateInfo;
	ModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ModuleCreateInfo.codeSize = Spirv.size() * sizeof(uint32_t);
	ModuleCreateInfo.pCode = Spirv.data();
	ModuleCreateInfo.pNext = nullptr;


	VK_CHECK( vkCreateShaderModule(*GRHI->GetDevice(), &ModuleCreateInfo, nullptr, &ShaderModule));

	return ShaderModule;
}
void
FRHIUtils::CreateBuffer(VkBufferUsageFlagBits Usage,
                        uint32 BufferSize,
                        VkFlags PropertyFlags,
                        VkBuffer &Buffer,
                        VkDeviceMemory &DeviceMemory)
{
	VkBufferCreateInfo BufferInfo {};

	BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferInfo.size = BufferSize;
	BufferInfo.usage = Usage;
	BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	auto Device = *GRHI->GetDevice();
	VK_CHECK(vkCreateBuffer(Device,&BufferInfo,nullptr,&Buffer));

	VkMemoryRequirements MemRequrements;

	vkGetBufferMemoryRequirements(Device,Buffer,&MemRequrements);

	VkMemoryAllocateInfo AllocInfo{};
	AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	AllocInfo.allocationSize = MemRequrements.size;
	AllocInfo.memoryTypeIndex =
		FRHIUtils::FindMemoryType(MemRequrements.memoryTypeBits,
								  PropertyFlags );
	VK_CHECK(vkAllocateMemory(Device,&AllocInfo,nullptr,&DeviceMemory));

	vkBindBufferMemory(Device,Buffer,DeviceMemory,0);
}
void
FRHIUtils::CreateImage(uint32 Height,
                       uint32 Width,
                       VkFormat Format,
                       VkImageTiling Tiling,
                       VkImageUsageFlags UsageFlags,
                       VkFlags PropertyFlags,
                       VkImage &Image,
                       VkDeviceMemory &DeviceMemory)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = Width;
	imageInfo.extent.height = Height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = Format;
	imageInfo.tiling = Tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = UsageFlags;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	auto Device = *GRHI->GetDevice();

	VK_CHECK(vkCreateImage(Device, &imageInfo, nullptr, &Image));

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(Device, Image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, PropertyFlags);

	vkAllocateMemory(Device, &allocInfo, nullptr, &DeviceMemory);
	vkBindImageMemory(Device, Image, DeviceMemory, 0);
}
VkCommandBuffer
FRHIUtils::BeginOneTimeCommandBuffer()
{
	auto Device = *GRHI->GetDevice();
	auto CommandBufferPool =  GEngine->GetRenderer()->GetCommandBufferPool();
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = CommandBufferPool->GetPool();
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(Device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void
FRHIUtils::EndOneTimeCommandBuffer(VkCommandBuffer CommandBuffer)
{
	vkEndCommandBuffer(CommandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandBuffer;

	auto Device = *GRHI->GetDevice();
	auto CommandBufferPool =  GEngine->GetRenderer()->GetCommandBufferPool();
	auto GraphicsQueue = *GRHI->GetGraphicsQueue();

	vkQueueSubmit(GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(GraphicsQueue);

	vkFreeCommandBuffers(Device, CommandBufferPool->GetPool() , 1, &CommandBuffer);
}

void
FRHIUtils::TransitionImageLayout(VkImage Image, VkFormat Format, VkImageLayout SrcLayout, VkImageLayout DstLayout)
{
	OneTimeCommand([&](VkCommandBuffer Buffer){
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = SrcLayout;
		barrier.newLayout = DstLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = Image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		barrier.srcAccessMask = 0; // TODO
		barrier.dstAccessMask = 0; // TODO

		vkCmdPipelineBarrier(
			Buffer,
			0 /* TODO */, 0 /* TODO */,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	});

}
void
FRHIUtils::CopyBufferToImage(VkImage Image, VkBuffer Buffer, uint32 Height, uint32 Width)
{
	OneTimeCommand([&](VkCommandBuffer CommandBuffer){
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = {0, 0, 0};
		region.imageExtent = {
			Width,
			Height,
			1
		};
		vkCmdCopyBufferToImage(
			CommandBuffer,
			Buffer,
			Image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

	});

}
