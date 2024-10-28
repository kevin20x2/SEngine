//
// Created by kevin on 2024/8/11.
//

#include "SwapChain.h"

#include "RHI.h"
#include "RHIUtils.h"
#include "volk.h"


class FSwapChainImp : public FSwapChain
{

public:
	void CreateSwapChain(VkExtent2D DisplaySize);
	void CreateImageViews();

	virtual VkFormat GetFormat() override
	{
		return ImageFormat;
	};
	virtual VkImageView * GetView(int32 Idx) override;
	virtual VkExtent2D GetExtent() override
	{
		return Extent;
	}

    virtual VkSwapchainKHR * GetSwapChain() override
	{
		return  &SwapChain;
	}
	virtual uint32 GetImageCount() const override
	{
		return Images.size();
	}
	~FSwapChainImp()
	{
		//CleanUp();
	}

	virtual void CleanUp() override;
private:
    VkSwapchainKHR SwapChain;
    VkExtent2D Extent;
    VkFormat ImageFormat;
    TArray<VkImage> Images;
    TArray <VkImageView> ImageViews;
};

void FSwapChainImp::CreateSwapChain(VkExtent2D DisplaySize)
{
	FSwapChainSupportDetails SwapChainSupport =
		FRHIUtils::QuerySwapChainSupport(*GRHI->GetPhysicalDevice(), *GRHI->GetCurSurface() );

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

	uint32_t ImageCount = SwapChainSupport.Capabilites.minImageCount ;
	if(SwapChainSupport.Capabilites.maxImageCount > 0 &&
	ImageCount > SwapChainSupport.Capabilites.maxImageCount )
	{
		ImageCount = SwapChainSupport.Capabilites.maxImageCount;
	}

	auto PreTransformFlag = SwapChainSupport.Capabilites.currentTransform;

	VkSwapchainCreateInfoKHR CreateInfo {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	CreateInfo.surface = *GRHI->GetCurSurface();
	CreateInfo.minImageCount = ImageCount;
	CreateInfo.imageFormat = SurfaceFormat.format;
	CreateInfo.imageColorSpace = SurfaceFormat.colorSpace;

	CreateInfo.imageExtent = DisplaySize;
	CreateInfo.imageArrayLayers = 1;
	CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	CreateInfo.preTransform = PreTransformFlag;

	auto Indices = FRHIUtils::FindQueueFamilies(
		*GRHI->GetPhysicalDevice(),*GRHI->GetCurSurface() );

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

	VK_CHECK(vkCreateSwapchainKHR(
		*GRHI->GetDevice() ,&CreateInfo,nullptr ,&SwapChain));

	vkGetSwapchainImagesKHR(*GRHI->GetDevice(),SwapChain,&ImageCount,nullptr);
	Images.resize(ImageCount);
	vkGetSwapchainImagesKHR(*GRHI->GetDevice(),SwapChain,&ImageCount,
		Images.data());

	ImageFormat = SurfaceFormat.format;
	Extent = DisplaySize;
}

void FSwapChainImp::CreateImageViews()
{
	ImageViews.resize(Images.size());
	for (uint32 i = 0; i < Images.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = Images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = ImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		VK_CHECK(vkCreateImageView(*GRHI->GetDevice(), &createInfo, nullptr,
		                           &ImageViews[i]));
	}
}

VkImageView* FSwapChainImp::GetView(int32 Idx)
{
	return &ImageViews[Idx];
}

void FSwapChainImp::CleanUp()
{
	for(uint32 i = 0 ; i < ImageViews.size() ; ++ i)
	{
		vkDestroyImageView(*GRHI->GetDevice(),ImageViews[i],nullptr);
	}

	vkDestroySwapchainKHR(*GRHI->GetDevice(),SwapChain,nullptr);
}

FSwapChain* FSwapChain::CreateSwapChain(VkExtent2D DisplaySize)
{
	FSwapChainImp * SwapChain = new FSwapChainImp;
	SwapChain->CreateSwapChain(DisplaySize);
	SwapChain->CreateImageViews();
	return SwapChain;
}
