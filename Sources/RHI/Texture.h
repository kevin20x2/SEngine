//
// Created by 郑文凯 on 2024/9/29.
//

#ifndef TEXTURE_H
#define TEXTURE_H
#include "vulkan/vulkan.h"
#include "Core/BaseTypes.h"

struct FTextureCreateParams
{
	uint32 BufferSize = 0;
	uint32 Height = 0;
	uint32 Width  = 0 ;
	void * BufferPtr = nullptr;
};

class FTexture
{
public:
	explicit FTexture(const FTextureCreateParams & Params);

protected:
	uint32 Height,Width;
	VkImage ImageHandle;
	VkBuffer BufferHandle;
	VkDeviceMemory DeviceMemory;
	VkDeviceMemory ImageMemory;

};


#endif //TEXTURE_H
