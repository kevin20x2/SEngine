#pragma once
#include <volk.h>
#include <optional>
#include <vector>



class KzVkEngine
{
public:

	static constexpr uint32_t MAX_FRAME_IN_FLIGHT = 2;

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;
		bool IsComplete() const
		{
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilites;
		std::vector <VkSurfaceFormatKHR> Formats;
		std::vector <VkPresentModeKHR> PresentModes;
	};

	//explicit KzVkEngine(KzPlatform * InPlatform );

	void InitVulkan();

	void CreateDevice();

	void CreateInstance();

	void CreateSurface();

	bool CheckValidationLayerSupport();

	bool IsDeviceSuitable(VkPhysicalDevice Device);

	std::vector <const char * >  GetRequiredExtensions(bool bEnableValidationLayer);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice Device);

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice Device);

	void CreateLogicalDeviceAndQueue();

	void EstablishDisplaySizeIdentity();
	
	void CreateDescriptorSetLayout();
	
	void CreateGraphicsPipeline();

	void CreateDescriptorPool();

	void CreateDescriptorSet();

	void CreateUniformBuffer();

	void CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, 
		VkMemoryPropertyFlags Properties, VkBuffer & Buffer, VkDeviceMemory & BufferMemory);


	void CreateSwapChain() ;

	void CreateImageViews();

	void CreateRenderPass();

	void CreateFrameBuffers();

	void CreateCommandPool();

	void CreateCommandBuffer();

	void CreateSyncObjects();


	void Render();

	void RecordCommandBuffer(VkCommandBuffer CommandBuffer, uint32_t ImageIndex);

	uint32_t FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties);

	VkInstance GetVkInstance() 
	{
		return Instance;
	}


protected:

	// Vulkan Variable

	VkInstance Instance;
	VkSurfaceKHR Surface;

	VkPhysicalDevice physicalDevice;

	VkDevice Device;

	VkQueue GraphicsQueue;
	VkQueue PresentQueue;

	VkSwapchainKHR SwapChain;

	VkRenderPass RenderPass;

	VkDescriptorSetLayout DescriptorSetLayout;
	VkDescriptorPool DescriptorPool;

	VkPipelineLayout PipelineLayout;
	VkPipeline GraphicPipeline;

	std::vector <VkDescriptorSet> DescriptorSets;

	std::vector <VkImage> SwapChainImages;
	std::vector <VkImageView> SwapChainImageViews;
	std::vector <VkFramebuffer> SwapChainFrameBuffers;
	VkCommandPool CommandPool;
	std::vector <VkCommandBuffer> CommandBuffers;

	std::vector <VkBuffer> UniformBuffers;
	std::vector <VkDeviceMemory> UniformBuffersMemory;

	VkFormat SwapChainImageFormat;
	VkExtent2D SwapChainExtent;
	VkExtent2D DisplaySizeIdentity;

	VkSurfaceTransformFlagBitsKHR PreTransformFlag;

	std::vector <VkSemaphore> ImageAvailableSemaphores;
	std::vector <VkSemaphore> RenderFinishSemaphores;
	std::vector <VkFence> InFlightFences;

	uint32_t CurrentFrame = 0;
	// End of Vulkan


	bool bEnableValidationLayer = false;


};