//
// Created by vinkzheng on 2025/8/12.
//

#include "TextureCube.h"

#include "RHI.h"
#include "RHIUtils.h"
#include "volk.h"

static uint32 ComputeTotalMips(uint32 Width , uint32 Height)
{
    if(Width <=0 || Height <=0) return 0;
    uint32 Size = max(Width,Height);
    uint32 Levels = 0 ;

    while(Size)
    {
        ++Levels;
        Size>>=1;
    }
    return Levels;
}


TSharedPtr<FTextureCubeRHI> FTextureCubeRHI::Create(const FTextureCubeCreateParams &Params)
{

    TSharedPtr<FTextureCubeRHI> Cube = std::make_shared<FTextureCubeRHI>();

    auto & MipLevels = Cube->MipLevels;
    MipLevels = min(Params.MaxMipLevels, ComputeTotalMips(Params.Width, Params.Height) );
    MipLevels = Params.bCreateMips ? MipLevels : 1;

    VkImageCreateInfo ImageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = Params.Format,
        .extent= { Params.Width, Params.Height,1 },
        .mipLevels = MipLevels,
        .arrayLayers = 6,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = Params.UsageFlags,
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
    vkBindImageMemory(Device,Cube->CubeImage,Cube->CubeMemory,0);


    /*
    FRHIUtils::CreateImage(Params.Height,Params.Width,Params.Format,VK_IMAGE_TILING_OPTIMAL,VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        Cube->CubeImage,Cube->CubeMemory
        );

    */
    VkDeviceSize LayerSize = Params.Width * Params.Height * FRHIUtils::FormatSize(Params.Format);

    VkDeviceSize TotalSize = LayerSize * 6;

    VkBuffer StagingBuffer ;
    VkDeviceMemory StagingBufferMemory;
    FRHIUtils::CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,TotalSize,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        StagingBuffer,StagingBufferMemory
        );

    void *TempData;

    vkMapMemory(Device ,StagingBufferMemory , 0 ,TotalSize,0,&TempData);

    //for(uint32_t layer = 0 ;layer < 6; layer++)
    {
        memcpy(static_cast<uint8*>(TempData) ,
            Params.Data
            ,static_cast<size_t>( TotalSize));
    }
    vkUnmapMemory(Device ,StagingBufferMemory);

    FRHIUtils::TransitionImageLayout(Cube->CubeImage,Params.Format,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,0,1,0,6);

    //FRHIUtils::CopyBufferToImage(Cube->CubeImage,StagingBuffer,Params.Height,Params.Width,6);
    FRHIUtils::OneTimeCommand([=](VkCommandBuffer CmdBuffer)
    {
        TArray <VkBufferImageCopy> CopyRegions;
        for(uint32_t Layer = 0 ;Layer < 6; Layer++)
        {
            VkBufferImageCopy Region {
                .bufferOffset = (VkDeviceSize)( Layer*LayerSize),
                .imageSubresource= { VK_IMAGE_ASPECT_COLOR_BIT, 0 , Layer, 1, },
                .imageOffset ={ 0, 0, 0 },
                .imageExtent = {Params.Width,Params.Height,1},
            };
            CopyRegions.Add(Region);
        }

        vkCmdCopyBufferToImage(CmdBuffer,StagingBuffer,Cube->CubeImage,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            CopyRegions.size(),CopyRegions.data());
    });

    FRHIUtils::TransitionImageLayout(Cube->CubeImage,Params.Format,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,0,1,0,6);
    FRHIUtils::TransitionImageLayout(Cube->CubeImage,Params.Format,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,1,MipLevels-1,0,6);


    VkImageViewCreateInfo ImageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = Cube->CubeImage,
        .viewType = VK_IMAGE_VIEW_TYPE_CUBE,
        .format = Params.Format,
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = MipLevels,
        .baseArrayLayer = 0,
        .layerCount = 6,
            }
    };

    VK_CHECK( vkCreateImageView(Device , &ImageViewCreateInfo,nullptr, &Cube->CubeImageView));

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

	VK_CHECK(vkCreateSampler(Device,&samplerInfo,nullptr,&Cube->Sampler));

    return Cube;
}

FTextureCubeRHI::~FTextureCubeRHI()
{
   auto Device = *GRHI->GetDevice();
   vkDestroyImageView(Device,CubeImageView,nullptr);
   vkDestroyImage(Device,CubeImage,nullptr);
   vkFreeMemory(Device,CubeMemory,nullptr);
}
