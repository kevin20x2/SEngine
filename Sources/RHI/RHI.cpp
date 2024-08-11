//
// Created by kevin on 2024/8/11.
//

#include "RHI.h"

#include <cassert>
#include <cstdlib>
#include <set>
#include <vector>

#include "volk.h"
#include "GLFW/glfw3.h"

#include "RHIUtils.h"
#include "Platform/Window.h"

FRHI * GRHI = nullptr;


class FRHIImp : public FRHI
{
public:
    virtual VkDevice * GetDevice() override
    {
        return & Device;
    }
    virtual VkInstance * GetInstance() override
    {
        return & Instance;
    }

	virtual VkPhysicalDevice * GetPhysicalDevice() override
    {
    	return & PhysicalDevice;
    }
	virtual VkSurfaceKHR * GetCurSurface() override
    {
    	return & Surface;
    }

	virtual uint32 GetMaxFrameInFlight() override
    {
    	return 2;
    }



    void Initialize(FWindow * InWindow);


protected:
    void CreateInstance();
	void CreatePhysicalDeivce();
	void CreateDevice();

	void EstablishDisplaySize();

#pragma region Defines
private:

    VkInstance Instance;
	VkPhysicalDevice PhysicalDevice;
    VkDevice Device;

	VkSurfaceKHR Surface;

	VkQueue GraphicsQueue;
	VkQueue PresentQueue;

	VkExtent2D DisplaySize;

#pragma endregion
};



void FRHIImp::Initialize(FWindow * InWindow)
{
	VK_CHECK(volkInitialize());
	CreateInstance();
	Surface = InWindow->CreateSurface(&Instance);
	CreatePhysicalDeivce();
	EstablishDisplaySize();

}

void FRHIImp::CreateInstance()
{
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


void FRHIImp::CreatePhysicalDeivce()
{
	uint32_t DeviceCount = 0;
	vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);

	std::vector<VkPhysicalDevice> Devices(DeviceCount);

	vkEnumeratePhysicalDevices(Instance, &DeviceCount,Devices.data());

	for (const auto& Device : Devices)
	{
		if(FRHIUtils::IsDeivceSuitable(Device,Surface))
		{
			PhysicalDevice = Device;
			break ;
		}
	}


	assert(PhysicalDevice != VK_NULL_HANDLE);
}

void FRHIImp::CreateDevice()
{
	auto Indices = FRHIUtils::FindQueueFamilies(PhysicalDevice,Surface);
	std::vector <VkDeviceQueueCreateInfo > QueueCreateInfos;

	std::set<uint32_t> UniqueQueueFamilies = {
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

	const std::vector <const char * > DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};


	VkPhysicalDeviceFeatures DeviceFeatures {};

	VkDeviceCreateInfo CreateInfo {};

	CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	CreateInfo.queueCreateInfoCount = static_cast <uint32_t>(QueueCreateInfos.size());
	CreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
	CreateInfo.pEnabledFeatures = & DeviceFeatures;
	CreateInfo.enabledExtensionCount = static_cast <uint32_t> (DeviceExtensions.size());
	CreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();
	CreateInfo.enabledLayerCount = 0 ;

	VK_CHECK(vkCreateDevice(PhysicalDevice ,&CreateInfo,nullptr ,&Device));

	vkGetDeviceQueue(Device, Indices.GraphicsFamily.value(),0,&GraphicsQueue);
	vkGetDeviceQueue(Device, Indices.PresentFamily.value(),0,&PresentQueue);
}

void FRHIImp::EstablishDisplaySize()
{
	VkSurfaceCapabilitiesKHR Capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice,Surface,&Capabilities);

	if(Capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR ||
		Capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR
		)
	{
		std::swap(Capabilities.currentExtent.height,Capabilities.currentExtent.width );
	}
	DisplaySize = Capabilities.currentExtent;
}

bool InitRHI(FWindow * InWindow)
{
    auto RHI = new FRHIImp();
    RHI->Initialize(InWindow);
    GRHI = RHI ;
	return true;
}
