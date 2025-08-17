//
// Created by vinkzheng on 2025/8/12.
//

#ifndef TEXTURECUBE_H
#define TEXTURECUBE_H
#include "Platform/CImgTextureLoader.h"


struct FTextureCubeCreateParams
{
    uint32_t Width , Height;
    VkFormat Format;
    uint8 * Data;
    VkImageUsageFlags UsageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    bool bCreateMips = false;
    uint32 MaxMipLevels = static_cast<uint32>(-1);
    bool bRenderTarget = false;
};

struct FTextureCubeData
{
    uint32 Width;
    uint32 Height;
    TArray <uint8> InData;
};

class FTextureCubeRHI : public FTextureBase
{
public:
    static TSharedPtr <FTextureCubeRHI> Create(const FTextureCubeCreateParams & Params);
    virtual ~FTextureCubeRHI();

    virtual VkImageView GetImageView() const override
    {
        return CubeImageView;
    }
    virtual VkSampler GetSampler() const override
    {
        return Sampler;
    }

    uint32 GetMipLevels() const
    {
        return MipLevels;
    }

    VkImage GetImage() const
    {
        return CubeImage;
    }

protected:



    VkImage CubeImage;
    VkDeviceMemory CubeMemory;
    VkImageView CubeImageView;
    VkSampler  Sampler;
    uint32 MipLevels;
};



#endif //TEXTURECUBE_H
