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

VkShaderModule FRHIUtils::LoadHlslShaderByFilePath(const std::string& FilePath, VkShaderStageFlagBits Stage)
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


	VkShaderModule ShaderModule;
	VkShaderModuleCreateInfo ModuleCreateInfo;
	ModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ModuleCreateInfo.codeSize = Spirv.size() * sizeof(uint32_t);
	ModuleCreateInfo.pCode = Spirv.data();
	ModuleCreateInfo.pNext = nullptr;


	VK_CHECK( vkCreateShaderModule(*GRHI->GetDevice(), &ModuleCreateInfo, nullptr, &ShaderModule));

	return ShaderModule;
}
