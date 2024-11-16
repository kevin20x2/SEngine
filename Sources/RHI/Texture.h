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
	explicit FTexture();
	virtual ~FTexture();

public:
	template <typename TextureType = FTexture>
	static TSharedPtr <TextureType>
	    CreateTexture(const FTextureCreateParams & Params)
	{
		auto Result = std::make_shared<TextureType>();
		Result->Initialize(Params);
		return Result;
	}

	virtual void Initialize(const FTextureCreateParams & Params);

	VkImageView GetImageView() const
	{
		return ImageView;
	}

	VkSampler GetSampler() const
	{
		return Sampler;
	}

protected:
	uint32 Height,Width;
	VkImage ImageHandle;
	VkBuffer BufferHandle;
	VkDeviceMemory DeviceMemory;
	VkDeviceMemory ImageMemory;


	VkImageView ImageView;

	VkSampler Sampler;
};

class FSampler
{
public:
	FSampler();
	void Initalize();
	virtual ~FSampler();

	VkSampler Sampler;
};


#endif //TEXTURE_H
