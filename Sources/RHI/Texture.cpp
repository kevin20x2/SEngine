//
// Created by 郑文凯 on 2024/9/29.
//

#include "Texture.h"
#include "RHI.h"
#include "volk.h"
#include "RHIUtils.h"

FTexture::FTexture(const FTextureCreateParams &Params)
{

	FRHIUtils::CreateBuffer(
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		Params.BufferSize,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		BufferHandle,
		DeviceMemory);

	if(Params.BufferPtr)
	{
		void *Data ;
		auto Device = GRHI->GetDevice();
		vkMapMemory(*Device,
					DeviceMemory,0,Params.BufferSize,0,&Data);
		memcpy(Data,Params.BufferPtr,Params.BufferSize);
		vkUnmapMemory(*Device,DeviceMemory);
	}

	FRHIUtils::CreateImage(Params.Height,Params.Width,
						   VK_FORMAT_R8G8B8A8_SRGB,
						   VK_IMAGE_TILING_OPTIMAL,
						   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
						   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ,
						   ImageHandle,
						   ImageMemory );

	FRHIUtils::TransitionImageLayout(ImageHandle,
									 VK_FORMAT_R8G8B8A8_SRGB,
									 VK_IMAGE_LAYOUT_UNDEFINED,
									 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	FRHIUtils::CopyBufferToImage(ImageHandle,BufferHandle,Params.Height,Params.Width);

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = ImageHandle;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	auto Device = *GRHI->GetDevice();

	VK_CHECK(vkCreateImageView(Device,&viewInfo,nullptr,&ImageView));

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	samplerInfo.anisotropyEnable = VK_FALSE;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	vkCreateSampler(Device,&samplerInfo,nullptr,&Sampler);
}
