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
};

class FTextureCubeRHI
{
public:
    static TSharedPtr <FTextureCubeRHI> Create(const FTextureCubeCreateParams & Params);
    virtual ~FTextureCubeRHI();
protected:



    VkImage CubeImage;
    VkDeviceMemory CubeMemory;
    VkImageView CubeImageView;

};



#endif //TEXTURECUBE_H
