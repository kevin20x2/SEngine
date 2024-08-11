#include "VkEngine.h"
#include <cassert>
#include <vector>
#include <cstdio>
#include <set>


#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/ResourceLimits.h>

#include <fstream>

#include "GLFW/glfw3.h"
#include "Platform/Window.h"

const std::vector <const char* > ValidationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector <const char * > DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


#define VK_CHECK(x) \
	do {			\
VkResult  error = x;\
	if (error)\
	{				\
				abort();\
	}				\
				\
	}while(0)\



static VkShaderModule LoadHlslShader(const std::string & filePath, VkShaderStageFlagBits Stage,VkDevice Device)
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
	std::ifstream ifs(filePath);

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


	VK_CHECK( vkCreateShaderModule(Device, &ModuleCreateInfo, nullptr, &ShaderModule));

	return ShaderModule;

}


KzVkEngine::KzVkEngine(FWindow* InWindow) :
Window(InWindow)
{
}

void KzVkEngine::InitVulkan()
{
	VK_CHECK(volkInitialize());
	CreateInstance();
	CreateSurface();
	CreateDevice();
	CreateLogicalDeviceAndQueue();
	EstablishDisplaySizeIdentity();
	CreateSwapChain();
	CreateImageViews();

	CreateDescriptorSetLayout();
	CreateUniformBuffer();
	CreateDescriptorPool();
	CreateDescriptorSet();

	CreateRenderPass();

	CreateGraphicsPipeline();

	CreateCommandPool();
	CreateCommandBuffer();
	CreateFrameBuffers();
	CreateSyncObjects();
}

void KzVkEngine::CreateDevice()
{
	uint32_t DeviceCount = 0;
	vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);


	std::vector<VkPhysicalDevice> Devices(DeviceCount);

	vkEnumeratePhysicalDevices(Instance, &DeviceCount,Devices.data());

	for (const auto& device : Devices)
	{
		if(IsDeviceSuitable(device))
		{
			physicalDevice = device;
			break ;
		}
	}

	//if(glfwGetPhysicalDevicePresentationSupport(Instance,physicalDevice,))

	assert(physicalDevice != VK_NULL_HANDLE);
}

void KzVkEngine::CreateInstance()
{

	assert(!bEnableValidationLayer || CheckValidationLayerSupport());

	uint32_t Count = 0;
	auto RequiredExtensions =  glfwGetRequiredInstanceExtensions(&Count); // Platform->GetRequiredExtension();//GetRequiredExtensions(bEnableValidationLayer);


	VkApplicationInfo AppInfo{  };
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pApplicationName = "zkVulkan";
	AppInfo.applicationVersion =  0 ;//VK_MAKE_VERSION(1, 0, 0);
	AppInfo.pEngineName = "No Engine";
	AppInfo.engineVersion = 0 ; //VK_MAKE_VERSION(1, 0, 0);
	AppInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &AppInfo;
	createInfo.enabledExtensionCount = Count;// RequiredExtensions.size();
	createInfo.ppEnabledExtensionNames = RequiredExtensions;// RequiredExtensions.data();
	createInfo.pApplicationInfo = &AppInfo;
	#ifdef VK_USE_PLATFORM_METAL_EXT
	createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
	#endif

	// not enable validationLayers
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;

	VkResult result = vkCreateInstance(&createInfo, nullptr, &Instance);
	if (result != VK_SUCCESS)
	{
		abort();
	}

	volkLoadInstance(Instance);


}

void KzVkEngine::CreateSurface()
{
	Surface = Window->CreateSurface(&Instance);
}

bool KzVkEngine::CheckValidationLayerSupport()
{
	uint32_t LayerCount;

	vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
	std::vector <VkLayerProperties> availableLayers(LayerCount);
	vkEnumerateInstanceLayerProperties(&LayerCount, availableLayers.data());

	for (const char* layerName : ValidationLayers)
	{
		bool bLayerFound = false;
		for (const auto& LayerProperties : availableLayers)
		{
			if (strcmp(layerName, LayerProperties.layerName) == 0)
			{
				bLayerFound = true;
				break;
			}
		}
		if (!bLayerFound)
		{
			return false;
		}
	}
	return true;
}

bool KzVkEngine::IsDeviceSuitable(VkPhysicalDevice Device)
{
	auto Indice = FindQueueFamilies(Device);
	bool SwapChainAdequate = false;

	auto SwapChainSupportDetail = QuerySwapChainSupport(Device);
	SwapChainAdequate = !SwapChainSupportDetail.Formats.empty() &&
						!SwapChainSupportDetail.PresentModes.empty();


	return Indice.IsComplete() && SwapChainAdequate;
}

KzVkEngine::QueueFamilyIndices
KzVkEngine::FindQueueFamilies(VkPhysicalDevice Device)
{
	QueueFamilyIndices Result;

	uint32_t QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount,nullptr);

	std::vector <VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilies.data());

	int i = 0 ;

	for(const auto &QueueFamily : QueueFamilies)
	{
		if(QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			Result.GraphicsFamily = i ;
		}

		VkBool32 PresentSupport =false;
		vkGetPhysicalDeviceSurfaceSupportKHR(Device,i , Surface,&PresentSupport);
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
KzVkEngine::SwapChainSupportDetails
KzVkEngine::QuerySwapChainSupport(VkPhysicalDevice Device)
{
	SwapChainSupportDetails Result;

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

std::vector <const char * >  KzVkEngine::GetRequiredExtensions(bool bEnableValidationLayer)
{
	std::vector <const char * > Extensions;

	Extensions.push_back("VK_KHR_surface");

#ifdef VK_USE_PLATFORM_METAL_EXT
	Extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
	Extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	Extensions.push_back("VK_EXT_metal_surface");
	//Extensions.push_back("VK_KHR_portability_subset");
#endif
	/*
	uint32_t count ;
	const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
	for(uint32_t i = 0 ;i < count ; ++ i)
	{
		Extensions.push_back(glfwExtensions[i]);
		printf("glfw Extensions : %s\n", glfwExtensions[i]);
	}
	*/
	if(bEnableValidationLayer)
	{
		Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return Extensions;
}

void KzVkEngine::CreateLogicalDeviceAndQueue()
{
	auto Indices = FindQueueFamilies(physicalDevice);
	std::vector <VkDeviceQueueCreateInfo > QueueCreateInfos;

	std::set <uint32_t> UniqueQueueFamilies = {
		Indices.GraphicsFamily.value(),
		Indices.PresentFamily.value() };

	float QueuePriority = 1.0f;
	for(uint32_t QueueFamily : UniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = QueueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &QueuePriority;
		QueueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures DeviceFeatures {};

	VkDeviceCreateInfo CreateInfo {};

	CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	CreateInfo.queueCreateInfoCount = static_cast <uint32_t>(QueueCreateInfos.size());
	CreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
	CreateInfo.pEnabledFeatures = & DeviceFeatures;
	CreateInfo.enabledExtensionCount = static_cast <uint32_t> (DeviceExtensions.size());
	CreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();
	CreateInfo.enabledLayerCount = 0 ;

	VK_CHECK(vkCreateDevice (physicalDevice ,&CreateInfo,nullptr ,&Device));

	vkGetDeviceQueue(Device, Indices.GraphicsFamily.value(),0,&GraphicsQueue);
	vkGetDeviceQueue(Device, Indices.PresentFamily.value(),0,&PresentQueue);

}

void KzVkEngine::CreateSwapChain()
{
	SwapChainSupportDetails SwapChainSupport =
		QuerySwapChainSupport(physicalDevice);

	auto chooseSwapSurfaceFormat =
		[](const std::vector <VkSurfaceFormatKHR> & AvailableFormats)
		{
			for(const auto & format : AvailableFormats )
			{
				if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
				format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
				)
				{
					return format;
				}
			}
			return AvailableFormats[0];
		};

	VkSurfaceFormatKHR SurfaceFormat =
		chooseSwapSurfaceFormat(SwapChainSupport.Formats);

	VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR;

	uint32_t ImageCount = SwapChainSupport.Capabilites.minImageCount + 1;
	if(SwapChainSupport.Capabilites.maxImageCount > 0 &&
	ImageCount > SwapChainSupport.Capabilites.maxImageCount )
	{
		ImageCount = SwapChainSupport.Capabilites.maxImageCount;
	}

	PreTransformFlag = SwapChainSupport.Capabilites.currentTransform;

	VkSwapchainCreateInfoKHR CreateInfo {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	CreateInfo.surface = Surface;
	CreateInfo.minImageCount = ImageCount;
	CreateInfo.imageFormat = SurfaceFormat.format;
	CreateInfo.imageColorSpace = SurfaceFormat.colorSpace;

	CreateInfo.imageExtent = DisplaySizeIdentity;
	CreateInfo.imageArrayLayers = 1;
	CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	CreateInfo.preTransform = PreTransformFlag;

	auto Indices = FindQueueFamilies(physicalDevice);

	uint32_t QueueFamilyIndices [] = { Indices.GraphicsFamily.value(),
									   Indices.PresentFamily.value() };

	if(Indices.GraphicsFamily != Indices.PresentFamily)
	{
		CreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		CreateInfo.queueFamilyIndexCount = 2;
		CreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
	}
	else
	{
		CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		CreateInfo.queueFamilyIndexCount = 0 ;
		CreateInfo.pQueueFamilyIndices = nullptr;
	}
	CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
	CreateInfo.presentMode = PresentMode;
	CreateInfo.clipped = VK_TRUE;
	CreateInfo.oldSwapchain = VK_NULL_HANDLE;

	VK_CHECK(vkCreateSwapchainKHR(Device,&CreateInfo,nullptr ,&SwapChain));

	vkGetSwapchainImagesKHR(Device,SwapChain,&ImageCount,nullptr);
	SwapChainImages.resize(ImageCount);
	vkGetSwapchainImagesKHR(Device,SwapChain,&ImageCount,SwapChainImages.data());

	SwapChainImageFormat = SurfaceFormat.format;

	SwapChainExtent = DisplaySizeIdentity;


}

void KzVkEngine::EstablishDisplaySizeIdentity()
{
	VkSurfaceCapabilitiesKHR Capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice,Surface,&Capabilities);

	if(Capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR ||
		Capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR
		)
	{
		std::swap(Capabilities.currentExtent.height,Capabilities.currentExtent.width );
	}
	DisplaySizeIdentity = Capabilities.currentExtent;

}

void KzVkEngine::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding UboLayoutBinding{};
	UboLayoutBinding.binding = 0;
	UboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	UboLayoutBinding.descriptorCount = 1;
	UboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	UboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo LayoutCreateInfo{};
	LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	LayoutCreateInfo.bindingCount = 1;
	LayoutCreateInfo.pBindings = &UboLayoutBinding;

	VK_CHECK(vkCreateDescriptorSetLayout(Device,&LayoutCreateInfo,nullptr,&DescriptorSetLayout));

}


void KzVkEngine::CreateGraphicsPipeline()
{

	VkShaderModule VertShaderModule = LoadHlslShader("../../shaders/test.vert", VK_SHADER_STAGE_VERTEX_BIT, Device);
	VkShaderModule FragShaderModule = LoadHlslShader("../../shaders/test.frag", VK_SHADER_STAGE_FRAGMENT_BIT, Device);

	VkPipelineShaderStageCreateInfo VertShaderStageInfo{};
	VertShaderStageInfo.sType =
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	VertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	VertShaderStageInfo.module = VertShaderModule;
	VertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo FragShaderStageInfo{};
	FragShaderStageInfo.sType =
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	FragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	FragShaderStageInfo.module = FragShaderModule;
	FragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo ShaderStages[] = { VertShaderStageInfo , FragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo VertexInputInfo{};

	VertexInputInfo.sType =
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VertexInputInfo.vertexBindingDescriptionCount = 0;
	VertexInputInfo.pVertexAttributeDescriptions = nullptr;
	VertexInputInfo.vertexAttributeDescriptionCount = 0;
	VertexInputInfo.pVertexAttributeDescriptions = nullptr;

	VkPipelineInputAssemblyStateCreateInfo InputAssembly{};
	InputAssembly.sType =
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	InputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo ViewportState{};
	ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	ViewportState.viewportCount = 1;
	ViewportState.scissorCount = 1;

	
	VkPipelineRasterizationStateCreateInfo RasterizationState{};
	RasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	RasterizationState.depthClampEnable = VK_FALSE;
	RasterizationState.rasterizerDiscardEnable = VK_FALSE;
	RasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
	RasterizationState.lineWidth = 1.0f;

	RasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
	RasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;

	RasterizationState.depthBiasEnable = VK_FALSE;
	RasterizationState.depthBiasConstantFactor = 0.0f;
	RasterizationState.depthBiasClamp = 0.0f;
	RasterizationState.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo Multisampling{};
	  Multisampling.sType =
		  VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	  Multisampling.sampleShadingEnable = VK_FALSE;
	  Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	  Multisampling.minSampleShading = 1.0f;
	  Multisampling.pSampleMask = nullptr;
	  Multisampling.alphaToCoverageEnable = VK_FALSE;
	  Multisampling.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState ColorBlendAttachment{};
  ColorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  ColorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo ColorBlending{};
  ColorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  ColorBlending.logicOpEnable = VK_FALSE;
  ColorBlending.logicOp = VK_LOGIC_OP_COPY;
  ColorBlending.attachmentCount = 1;
  ColorBlending.pAttachments = &ColorBlendAttachment;
  ColorBlending.blendConstants[0] = 0.0f;
  ColorBlending.blendConstants[1] = 0.0f;
  ColorBlending.blendConstants[2] = 0.0f;
  ColorBlending.blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
  PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  PipelineLayoutInfo.setLayoutCount = 1;
  PipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;
  PipelineLayoutInfo.pushConstantRangeCount = 0;
  PipelineLayoutInfo.pPushConstantRanges = nullptr;

  VK_CHECK(vkCreatePipelineLayout(Device, &PipelineLayoutInfo, nullptr,
                                  &PipelineLayout));
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicStateCI{};
  dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateCI.pDynamicStates = dynamicStateEnables.data();
  dynamicStateCI.dynamicStateCount =
      static_cast<uint32_t>(dynamicStateEnables.size());

  VkGraphicsPipelineCreateInfo PipelineInfo{};
  PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  PipelineInfo.stageCount = 2;
  PipelineInfo.pStages = ShaderStages;
  PipelineInfo.pVertexInputState = &VertexInputInfo;
  PipelineInfo.pInputAssemblyState = &InputAssembly;
  PipelineInfo.pViewportState = &ViewportState;
  PipelineInfo.pRasterizationState = &RasterizationState;
  PipelineInfo.pMultisampleState = &Multisampling;
  PipelineInfo.pDepthStencilState = nullptr;
  PipelineInfo.pColorBlendState = &ColorBlending;
  PipelineInfo.pDynamicState = &dynamicStateCI;
  PipelineInfo.layout = PipelineLayout;
  PipelineInfo.renderPass = RenderPass;
  PipelineInfo.subpass = 0;
  PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  PipelineInfo.basePipelineIndex = -1;

  VK_CHECK(vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &PipelineInfo,
                                     nullptr, &GraphicPipeline));




}

void KzVkEngine::CreateDescriptorPool()
{
	VkDescriptorPoolSize PoolSize{};
	PoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	PoolSize.descriptorCount = static_cast <uint32_t>(MAX_FRAME_IN_FLIGHT);

	VkDescriptorPoolCreateInfo PoolInfo{};
	PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	PoolInfo.poolSizeCount = 1;
	PoolInfo.pPoolSizes = &PoolSize;
	PoolInfo.maxSets = static_cast <uint32_t>(MAX_FRAME_IN_FLIGHT); 
	VK_CHECK(vkCreateDescriptorPool(Device, &PoolInfo, nullptr, &DescriptorPool));

}

void KzVkEngine::CreateDescriptorSet()
{
	std::vector <VkDescriptorSetLayout> Layouts(MAX_FRAME_IN_FLIGHT, DescriptorSetLayout);

	VkDescriptorSetAllocateInfo AllocInfo{};
	AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	AllocInfo.descriptorPool = DescriptorPool;
	AllocInfo.descriptorSetCount = static_cast <uint32_t>(MAX_FRAME_IN_FLIGHT);
	AllocInfo.pSetLayouts = Layouts.data();

	DescriptorSets.resize(MAX_FRAME_IN_FLIGHT);
	VK_CHECK(vkAllocateDescriptorSets(Device, &AllocInfo, DescriptorSets.data()));

	for (size_t i = 0; i < MAX_FRAME_IN_FLIGHT; ++i)
	{
		VkDescriptorBufferInfo BufferInfo{};
		BufferInfo.buffer = UniformBuffers[i];
		BufferInfo.offset = 0;
		BufferInfo.range = 16 * sizeof(float);

		VkWriteDescriptorSet DescriptorWrite{};
		DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		DescriptorWrite.dstSet = DescriptorSets[i];
		DescriptorWrite.dstBinding = 0;
		DescriptorWrite.dstArrayElement = 0;
		DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		DescriptorWrite.descriptorCount = 1;
		DescriptorWrite.pBufferInfo = &BufferInfo;

		vkUpdateDescriptorSets(Device, 1, &DescriptorWrite, 0, nullptr);

	}

}

void KzVkEngine::CreateUniformBuffer()
{
	VkDeviceSize BufferSize = sizeof(float) * 16; // mvp matrix

	UniformBuffers.resize(MAX_FRAME_IN_FLIGHT);
	UniformBuffersMemory.resize(MAX_FRAME_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAME_IN_FLIGHT; ++i)
	{
		CreateBuffer(BufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ,
			UniformBuffers[i],UniformBuffersMemory[i]
			);

	}


}

void KzVkEngine::CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer& Buffer, VkDeviceMemory& BufferMemory)
{
	VkBufferCreateInfo BufferInfo{  };

	BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferInfo.size = Size;
	BufferInfo.usage = Usage;
	// TODO:??? 
	BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK(vkCreateBuffer(Device, &BufferInfo, nullptr, &Buffer));

	VkMemoryRequirements MemRequrements;
	vkGetBufferMemoryRequirements(Device, Buffer, &MemRequrements);

	VkMemoryAllocateInfo AllocInfo{};
	AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	AllocInfo.allocationSize = MemRequrements.size;
	AllocInfo.memoryTypeIndex =
		FindMemoryType(MemRequrements.memoryTypeBits, Properties);
	VK_CHECK(vkAllocateMemory(Device, &AllocInfo, nullptr, &BufferMemory));

	vkBindBufferMemory(Device, Buffer, BufferMemory, 0);


}

void KzVkEngine::Render()
{
	vkWaitForFences(Device,1, &InFlightFences[CurrentFrame], VK_TRUE , UINT64_MAX);

	uint32_t ImageIndex ;
	VkResult Result = vkAcquireNextImageKHR(
		Device,SwapChain,UINT64_MAX,ImageAvailableSemaphores[CurrentFrame],
		VK_NULL_HANDLE,&ImageIndex );

	vkResetFences(Device,1,&InFlightFences[CurrentFrame]);

	RecordCommandBuffer(CommandBuffers[CurrentFrame],ImageIndex);
	// Submit
	VkSubmitInfo submitInfo {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = {ImageAvailableSemaphores[CurrentFrame]};
	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandBuffers[CurrentFrame];

	VkSemaphore SignalSemaphores[] = { RenderFinishSemaphores[CurrentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = SignalSemaphores;

	VK_CHECK(vkQueueSubmit(GraphicsQueue,1,&submitInfo,InFlightFences[CurrentFrame]));

	// Present
	VkPresentInfoKHR PresentInfo = {};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = SignalSemaphores;

	VkSwapchainKHR swapChains [] = {SwapChain};

	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = swapChains;
	PresentInfo.pImageIndices = &ImageIndex;
	PresentInfo.pResults = nullptr;

	Result = vkQueuePresentKHR(PresentQueue,&PresentInfo);

	CurrentFrame = (CurrentFrame + 1) %MAX_FRAME_IN_FLIGHT;
}

void KzVkEngine::CreateSyncObjects()
{
	ImageAvailableSemaphores.resize(MAX_FRAME_IN_FLIGHT);
	RenderFinishSemaphores.resize(MAX_FRAME_IN_FLIGHT);
	InFlightFences.resize(MAX_FRAME_IN_FLIGHT);

	VkSemaphoreCreateInfo SemaphoreInfo {};
	SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo FenceInfo {};
	FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(size_t i = 0 ;i < MAX_FRAME_IN_FLIGHT; ++ i)
	{
		VK_CHECK(vkCreateSemaphore(Device, &SemaphoreInfo,nullptr, &ImageAvailableSemaphores[i]));

		VK_CHECK(vkCreateSemaphore(Device, &SemaphoreInfo,nullptr, &RenderFinishSemaphores[i]));

		VK_CHECK(vkCreateFence(Device,&FenceInfo,nullptr,&InFlightFences[i]));
	}


}
void KzVkEngine::CreateImageViews()
{
	SwapChainImageViews.resize(SwapChainImages.size());
	for (size_t i = 0; i < SwapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = SwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = SwapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		VK_CHECK(vkCreateImageView(Device, &createInfo, nullptr,
		                           &SwapChainImageViews[i]));
	}
}

void KzVkEngine::CreateFrameBuffers()
{
	SwapChainFrameBuffers.resize(SwapChainImageViews.size());
	for (size_t i = 0; i < SwapChainImageViews.size(); i++) {
		VkImageView attachments[] = {SwapChainImageViews[i]};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = RenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = SwapChainExtent.width;
		framebufferInfo.height = SwapChainExtent.height;
		framebufferInfo.layers = 1;

		VK_CHECK(vkCreateFramebuffer(Device, &framebufferInfo, nullptr,
		                             &SwapChainFrameBuffers[i]));
	}

}

void KzVkEngine::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice);
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();
	VK_CHECK(vkCreateCommandPool(Device, &poolInfo, nullptr, &CommandPool));
}

void KzVkEngine::CreateCommandBuffer()
{
	CommandBuffers.resize(MAX_FRAME_IN_FLIGHT);
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = CommandBuffers.size();

	VK_CHECK(vkAllocateCommandBuffers(Device, &allocInfo, CommandBuffers.data()));

}

void KzVkEngine::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = SwapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VK_CHECK(vkCreateRenderPass(Device, &renderPassInfo, nullptr, &RenderPass));

}

void KzVkEngine::RecordCommandBuffer(VkCommandBuffer CommandBuffer, uint32_t ImageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	VK_CHECK(vkBeginCommandBuffer(CommandBuffer, &beginInfo));

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = RenderPass;
	renderPassInfo.framebuffer = SwapChainFrameBuffers[ImageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = SwapChainExtent;

	VkViewport viewport{};
	viewport.width = (float)SwapChainExtent.width;
	viewport.height = (float)SwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.extent = SwapChainExtent;

	static float grey;
	grey += 0.005f;
	if (grey > 1.0f) {
		grey = 0.0f;
	}
	VkClearValue clearColor = {{{grey, grey, grey, 1.0f}}};

	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo,
	                     VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
	                  GraphicPipeline);
	vkCmdSetScissor(CommandBuffer, 0, 1, &scissor);
	vkCmdSetViewport(CommandBuffer, 0, 1, &viewport);
	//vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
	  //                      PipelineLayout, 0, 1, &DescriptorSets[CurrentFrame],
	    //                    0, nullptr);

	vkCmdDraw(CommandBuffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(CommandBuffer);
	VK_CHECK(vkEndCommandBuffer(CommandBuffer));

}

uint32_t KzVkEngine::FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties)
{
	VkPhysicalDeviceMemoryProperties MemProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &MemProperties);

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

