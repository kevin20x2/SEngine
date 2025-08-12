//
// Created by vinkzheng on 2025/8/12.
//

#include "TextureCube.h"

#include "RHI.h"
#include "RHIUtils.h"
#include "volk.h"

TSharedPtr<FTextureCubeRHI> FTextureCubeRHI::Create(const FTextureCubeCreateParams &Params)
{

    TSharedPtr<FTextureCubeRHI> Cube = std::make_shared<FTextureCubeRHI>();

    VkImageCreateInfo ImageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = Params.Format,
        .extent= { Params.Width, Params.Height,1 },
        .mipLevels = 1,
        .arrayLayers = 6,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };


   auto Device = *GRHI->GetDevice();
   VK_CHECK( vkCreateImage( Device ,  & ImageInfo , nullptr,  &Cube->CubeImage));

    VkMemoryRequirements MemoryRequirements;
    vkGetImageMemoryRequirements( Device , Cube->CubeImage, &MemoryRequirements);

    VkMemoryAllocateInfo MemoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = MemoryRequirements.size,
        .memoryTypeIndex = FRHIUtils::FindMemoryType(MemoryRequirements.memoryTypeBits,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        };
    VK_CHECK( vkAllocateMemory(Device,&MemoryAllocateInfo, nullptr, &Cube->CubeMemory));

}
