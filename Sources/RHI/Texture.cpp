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
						   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						   ImageHandle,
						   ImageMemory );

	FRHIUtils::TransitionImageLayout(ImageHandle,
									 VK_FORMAT_R8G8B8A8_SRGB,
									 VK_IMAGE_LAYOUT_UNDEFINED,
									 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	FRHIUtils::CopyBufferToImage(ImageHandle,BufferHandle,Params.Height,Params.Width);



}
