//
// Created by kevin on 2024/8/11.
//

#include "RHIUtils.h"

#include <DirStackFileIncluder.h>
#include <fstream>
#include <ResourceLimits.h>
#include <vector>
#include <spdlog/spdlog.h>

#include "volk.h"
#include "glslang/Public/ShaderLang.h"
#include "SPIRV/GlslangToSpv.h"
#include "CoreObjects/Engine.h"
#include "spirv_reflect.h"
#include "Platform/Path.h"

bool FRHIUtils::IsDeivceSuitable(VkPhysicalDevice Device, VkSurfaceKHR Surface,uint32 & OutIndex)
{
	auto Indice = FindQueueFamilies(Device,Surface);
	bool SwapChainAdequate = false;

	auto SwapChainSupportDetail = QuerySwapChainSupport(Device,Surface);
	SwapChainAdequate = !SwapChainSupportDetail.Formats.empty() &&
						!SwapChainSupportDetail.PresentModes.empty();

	OutIndex = Indice.GraphicsFamily.value();

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

VkShaderModule FRHIUtils::LoadHlslShaderByFilePath(const std::string& FilePath, VkShaderStageFlagBits Stage
												   ,TArray <FDescriptorSetLayoutInfo> & LayoutInfos,
												   FVertexInputInfo & OutVertexInputInfo )
{

	std::vector <uint32_t> Spirv;
	std::string InfoLog;

	glslang::InitializeProcess();

	auto Messages = static_cast<EShMessages>(EShMsgReadHlsl | EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);

	EShLanguage Language{};
	FString ShaderEntryName = GetShaderEntryPointByShaderStage(Stage);

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
	Shader.setEntryPoint(ShaderEntryName.c_str());
	Shader.setSourceEntryPoint(ShaderEntryName.c_str());
	Shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);
	Shader.setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_0);

	DirStackFileIncluder Includer;
	Includer.pushExternalLocalDirectory(FPath::GetDirectoryFromPath(FilePath));

	if (!Shader.parse(&glslang::DefaultTBuiltInResource, 100, false, Messages,Includer))
	{
		spdlog::critical("Failed to parse HLSL Shader ,Error {} \n  {}", Shader.getInfoLog(), Shader.getInfoDebugLog());
		//throw std::runtime_error("Fail to parse HLSL Shader");
	}

	glslang::TProgram Program;

	Program.addShader(&Shader);

	if (!Program.link(Messages))
	{
		printf("Failed to parse HLSL Shader ,Error %s \n  %s", Shader.getInfoDebugLog(), Shader.getInfoDebugLog());
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
		//input

		uint32 InputCount = 0;

		Result = spvReflectEnumerateInputVariables(&ReflectShaderModule,&InputCount,nullptr);
		assert(Result == SPV_REFLECT_RESULT_SUCCESS);
		TArray <SpvReflectInterfaceVariable * > InputVars(InputCount);
		Result = spvReflectEnumerateInputVariables(&ReflectShaderModule,&InputCount,InputVars.data());
		if(ReflectShaderModule.shader_stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
		{
			 OutVertexInputInfo =  GenerateVertexInputStateCreateInfo(InputVars);
		}
		// descriptor
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

FString FRHIUtils::GetShaderEntryPointByShaderStage(VkShaderStageFlagBits Stage)
{
	FString ShaderEntryName = "";

	switch (Stage)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		ShaderEntryName = "VertexMain";
		break;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		ShaderEntryName = "FragmentMain";
		break;
	default:
		break;
	}
	return ShaderEntryName;
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
		VkPipelineStageFlags sourceStage = 0;
		VkPipelineStageFlags destinationStage = 0 ;
		if(SrcLayout == VK_IMAGE_LAYOUT_UNDEFINED && DstLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (HasStencilComponent(Format)) {
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}

		vkCmdPipelineBarrier(
			Buffer,
			sourceStage /* TODO */, destinationStage /* TODO */,
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

static uint32_t FormatSize(VkFormat format) {
	uint32_t result = 0;
	switch (format) {
	case VK_FORMAT_UNDEFINED:
		result = 0;
		break;
	case VK_FORMAT_R4G4_UNORM_PACK8:
		result = 1;
		break;
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
		result = 2;
		break;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
		result = 2;
		break;
	case VK_FORMAT_R5G6B5_UNORM_PACK16:
		result = 2;
		break;
	case VK_FORMAT_B5G6R5_UNORM_PACK16:
		result = 2;
		break;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
		result = 2;
		break;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
		result = 2;
		break;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
		result = 2;
		break;
	case VK_FORMAT_R8_UNORM:
		result = 1;
		break;
	case VK_FORMAT_R8_SNORM:
		result = 1;
		break;
	case VK_FORMAT_R8_USCALED:
		result = 1;
		break;
	case VK_FORMAT_R8_SSCALED:
		result = 1;
		break;
	case VK_FORMAT_R8_UINT:
		result = 1;
		break;
	case VK_FORMAT_R8_SINT:
		result = 1;
		break;
	case VK_FORMAT_R8_SRGB:
		result = 1;
		break;
	case VK_FORMAT_R8G8_UNORM:
		result = 2;
		break;
	case VK_FORMAT_R8G8_SNORM:
		result = 2;
		break;
	case VK_FORMAT_R8G8_USCALED:
		result = 2;
		break;
	case VK_FORMAT_R8G8_SSCALED:
		result = 2;
		break;
	case VK_FORMAT_R8G8_UINT:
		result = 2;
		break;
	case VK_FORMAT_R8G8_SINT:
		result = 2;
		break;
	case VK_FORMAT_R8G8_SRGB:
		result = 2;
		break;
	case VK_FORMAT_R8G8B8_UNORM:
		result = 3;
		break;
	case VK_FORMAT_R8G8B8_SNORM:
		result = 3;
		break;
	case VK_FORMAT_R8G8B8_USCALED:
		result = 3;
		break;
	case VK_FORMAT_R8G8B8_SSCALED:
		result = 3;
		break;
	case VK_FORMAT_R8G8B8_UINT:
		result = 3;
		break;
	case VK_FORMAT_R8G8B8_SINT:
		result = 3;
		break;
	case VK_FORMAT_R8G8B8_SRGB:
		result = 3;
		break;
	case VK_FORMAT_B8G8R8_UNORM:
		result = 3;
		break;
	case VK_FORMAT_B8G8R8_SNORM:
		result = 3;
		break;
	case VK_FORMAT_B8G8R8_USCALED:
		result = 3;
		break;
	case VK_FORMAT_B8G8R8_SSCALED:
		result = 3;
		break;
	case VK_FORMAT_B8G8R8_UINT:
		result = 3;
		break;
	case VK_FORMAT_B8G8R8_SINT:
		result = 3;
		break;
	case VK_FORMAT_B8G8R8_SRGB:
		result = 3;
		break;
	case VK_FORMAT_R8G8B8A8_UNORM:
		result = 4;
		break;
	case VK_FORMAT_R8G8B8A8_SNORM:
		result = 4;
		break;
	case VK_FORMAT_R8G8B8A8_USCALED:
		result = 4;
		break;
	case VK_FORMAT_R8G8B8A8_SSCALED:
		result = 4;
		break;
	case VK_FORMAT_R8G8B8A8_UINT:
		result = 4;
		break;
	case VK_FORMAT_R8G8B8A8_SINT:
		result = 4;
		break;
	case VK_FORMAT_R8G8B8A8_SRGB:
		result = 4;
		break;
	case VK_FORMAT_B8G8R8A8_UNORM:
		result = 4;
		break;
	case VK_FORMAT_B8G8R8A8_SNORM:
		result = 4;
		break;
	case VK_FORMAT_B8G8R8A8_USCALED:
		result = 4;
		break;
	case VK_FORMAT_B8G8R8A8_SSCALED:
		result = 4;
		break;
	case VK_FORMAT_B8G8R8A8_UINT:
		result = 4;
		break;
	case VK_FORMAT_B8G8R8A8_SINT:
		result = 4;
		break;
	case VK_FORMAT_B8G8R8A8_SRGB:
		result = 4;
		break;
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A2R10G10B10_SINT_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		result = 4;
		break;
	case VK_FORMAT_A2B10G10R10_SINT_PACK32:
		result = 4;
		break;
	case VK_FORMAT_R16_UNORM:
		result = 2;
		break;
	case VK_FORMAT_R16_SNORM:
		result = 2;
		break;
	case VK_FORMAT_R16_USCALED:
		result = 2;
		break;
	case VK_FORMAT_R16_SSCALED:
		result = 2;
		break;
	case VK_FORMAT_R16_UINT:
		result = 2;
		break;
	case VK_FORMAT_R16_SINT:
		result = 2;
		break;
	case VK_FORMAT_R16_SFLOAT:
		result = 2;
		break;
	case VK_FORMAT_R16G16_UNORM:
		result = 4;
		break;
	case VK_FORMAT_R16G16_SNORM:
		result = 4;
		break;
	case VK_FORMAT_R16G16_USCALED:
		result = 4;
		break;
	case VK_FORMAT_R16G16_SSCALED:
		result = 4;
		break;
	case VK_FORMAT_R16G16_UINT:
		result = 4;
		break;
	case VK_FORMAT_R16G16_SINT:
		result = 4;
		break;
	case VK_FORMAT_R16G16_SFLOAT:
		result = 4;
		break;
	case VK_FORMAT_R16G16B16_UNORM:
		result = 6;
		break;
	case VK_FORMAT_R16G16B16_SNORM:
		result = 6;
		break;
	case VK_FORMAT_R16G16B16_USCALED:
		result = 6;
		break;
	case VK_FORMAT_R16G16B16_SSCALED:
		result = 6;
		break;
	case VK_FORMAT_R16G16B16_UINT:
		result = 6;
		break;
	case VK_FORMAT_R16G16B16_SINT:
		result = 6;
		break;
	case VK_FORMAT_R16G16B16_SFLOAT:
		result = 6;
		break;
	case VK_FORMAT_R16G16B16A16_UNORM:
		result = 8;
		break;
	case VK_FORMAT_R16G16B16A16_SNORM:
		result = 8;
		break;
	case VK_FORMAT_R16G16B16A16_USCALED:
		result = 8;
		break;
	case VK_FORMAT_R16G16B16A16_SSCALED:
		result = 8;
		break;
	case VK_FORMAT_R16G16B16A16_UINT:
		result = 8;
		break;
	case VK_FORMAT_R16G16B16A16_SINT:
		result = 8;
		break;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		result = 8;
		break;
	case VK_FORMAT_R32_UINT:
		result = 4;
		break;
	case VK_FORMAT_R32_SINT:
		result = 4;
		break;
	case VK_FORMAT_R32_SFLOAT:
		result = 4;
		break;
	case VK_FORMAT_R32G32_UINT:
		result = 8;
		break;
	case VK_FORMAT_R32G32_SINT:
		result = 8;
		break;
	case VK_FORMAT_R32G32_SFLOAT:
		result = 8;
		break;
	case VK_FORMAT_R32G32B32_UINT:
		result = 12;
		break;
	case VK_FORMAT_R32G32B32_SINT:
		result = 12;
		break;
	case VK_FORMAT_R32G32B32_SFLOAT:
		result = 12;
		break;
	case VK_FORMAT_R32G32B32A32_UINT:
		result = 16;
		break;
	case VK_FORMAT_R32G32B32A32_SINT:
		result = 16;
		break;
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		result = 16;
		break;
	case VK_FORMAT_R64_UINT:
		result = 8;
		break;
	case VK_FORMAT_R64_SINT:
		result = 8;
		break;
	case VK_FORMAT_R64_SFLOAT:
		result = 8;
		break;
	case VK_FORMAT_R64G64_UINT:
		result = 16;
		break;
	case VK_FORMAT_R64G64_SINT:
		result = 16;
		break;
	case VK_FORMAT_R64G64_SFLOAT:
		result = 16;
		break;
	case VK_FORMAT_R64G64B64_UINT:
		result = 24;
		break;
	case VK_FORMAT_R64G64B64_SINT:
		result = 24;
		break;
	case VK_FORMAT_R64G64B64_SFLOAT:
		result = 24;
		break;
	case VK_FORMAT_R64G64B64A64_UINT:
		result = 32;
		break;
	case VK_FORMAT_R64G64B64A64_SINT:
		result = 32;
		break;
	case VK_FORMAT_R64G64B64A64_SFLOAT:
		result = 32;
		break;
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		result = 4;
		break;
	case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
		result = 4;
		break;

	default:
		break;
	}
	return result;
}
//VkVertexInputBindingDescription
FVertexInputInfo
FRHIUtils::GenerateVertexInputStateCreateInfo(TArray<SpvReflectInterfaceVariable *> InputVars)
{
	//if (module.shader_stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT) {
		// Demonstrates how to generate all necessary data structures to populate
		// a VkPipelineVertexInputStateCreateInfo structure, given the module's
		// expected input variables.
		//
		// Simplifying assumptions:
		// - All vertex input attributes are sourced from a single vertex buffer,
		//   bound to VB slot 0.
		// - Each vertex's attribute are laid out in ascending order by location.
		// - The format of each attribute matches its usage in the shader;
		//   float4 -> VK_FORMAT_R32G32B32A32_FLOAT, etc. No attribute compression
		//   is applied.
		// - All attributes are provided per-vertex, not per-instance.
		VkVertexInputBindingDescription binding_description = {};
		binding_description.binding = 0;
		binding_description.stride = 0;  // computed below
		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
		TArray<VkVertexInputAttributeDescription> attribute_descriptions;
		attribute_descriptions.reserve(InputVars.size());
		for (uint32 i_var = 0; i_var < InputVars.size(); ++i_var) {
			const SpvReflectInterfaceVariable& refl_var = *(InputVars[i_var]);
			// ignore built-in variables
			if (refl_var.decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) {
				continue;
			}
			VkVertexInputAttributeDescription attr_desc{};
			attr_desc.location = refl_var.location;
			attr_desc.binding = binding_description.binding;
			attr_desc.format = static_cast<VkFormat>(refl_var.format);
			attr_desc.offset = 0;  // final offset computed below after sorting.
			attribute_descriptions.push_back(attr_desc);
		}
		// Sort attributes by location
		std::sort(std::begin(attribute_descriptions), std::end(attribute_descriptions),
		          [](const VkVertexInputAttributeDescription& a, const VkVertexInputAttributeDescription& b) {
			          return a.location < b.location;
		          });
		// Compute final offsets of each attribute, and total vertex stride.
		for (auto& attribute : attribute_descriptions) {
			uint32_t format_size = FormatSize(attribute.format);
			attribute.offset = binding_description.stride;
			binding_description.stride += format_size;
		}
		// Nothing further is done with attribute_descriptions or
		// binding_description in this sample. A real application would probably
		// derive this information from its mesh format(s); a similar mechanism
		// could be used to ensure mesh/shader compatibility.
		return  {vertex_input_state_create_info,
				 {binding_description },
				 attribute_descriptions } ;
}
VkFormat
FRHIUtils::FindSupportFormat(const TArray<VkFormat> &Formats,
                             VkImageTiling ImageTiling,
                             VkFormatFeatureFlags FeatureFlags)
{
	for(auto Format : Formats)
	{
		VkFormatProperties Properties;
		vkGetPhysicalDeviceFormatProperties(*GRHI->GetPhysicalDevice(),Format , & Properties);
		if(ImageTiling == VK_IMAGE_TILING_LINEAR &&
			(Properties.linearTilingFeatures & FeatureFlags) == FeatureFlags  )
		{
			return Format;
		}
		if(ImageTiling == VK_IMAGE_TILING_OPTIMAL && (
			(Properties.optimalTilingFeatures & FeatureFlags) == FeatureFlags ))
		{
			return Format;
		}
	}

	throw std::runtime_error("cant find support format");
	return VK_FORMAT_R32_UINT;
}
VkFormat
FRHIUtils::FindDepthFormat()
{
	return FindSupportFormat({
		VK_FORMAT_D24_UNORM_S8_UINT ,
		VK_FORMAT_D32_SFLOAT ,
		VK_FORMAT_D32_SFLOAT_S8_UINT
	}, VK_IMAGE_TILING_OPTIMAL,VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}
void
FRHIUtils::CreateImageView(VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags,VkImageView & ImageView)
{
	VkImageViewCreateInfo ViewInfo {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = Image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = Format,
		.subresourceRange =
			{
			.aspectMask = AspectFlags  ,
			.baseMipLevel = 0 ,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
			}
	};

	VK_CHECK(vkCreateImageView(*GRHI->GetDevice(),&ViewInfo,nullptr, &ImageView));
}
bool
FRHIUtils::HasStencilComponent(VkFormat format)
{
	//return false;
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}
