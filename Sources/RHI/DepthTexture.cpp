//
// Created by 郑文凯 on 2024/10/2.
//

#include "DepthTexture.h"
#include "RHIUtils.h"
void
FDepthTexture::Initialize(const FTextureCreateParams &Params)
{

	VkFormat DepthFormat = FRHIUtils::FindDepthFormat();

	FRHIUtils::CreateImage(Params.Height,Params.Width,
						   DepthFormat ,VK_IMAGE_TILING_OPTIMAL,
						   VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
						   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						   ImageHandle,ImageMemory );
	FRHIUtils::CreateImageView(ImageHandle,DepthFormat,VK_IMAGE_ASPECT_DEPTH_BIT , ImageView ) ;

	FRHIUtils::TransitionImageLayout(ImageHandle,DepthFormat,VK_IMAGE_LAYOUT_UNDEFINED , VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}
