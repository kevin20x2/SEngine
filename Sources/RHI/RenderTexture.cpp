//
// Created by kevin on 2024/12/6.
//

#include "RenderTexture.h"

#include "RHIUtils.h"

void FRenderTexture::Initialize(const FTextureCreateParams& Params)
{
    //FTexture::Initialize(Params);

    auto ImageFormat = VK_FORMAT_R8G8B8A8_SRGB;

    FRHIUtils::CreateImage(Params.Height,Params.Width,
         ImageFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        ImageHandle,ImageMemory );

    FRHIUtils::CreateImageView(ImageHandle,ImageFormat,VK_IMAGE_ASPECT_COLOR_BIT,ImageView);

}
