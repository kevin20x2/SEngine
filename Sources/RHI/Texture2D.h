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

class FTextureBase
{
	public:
	virtual VkImageView GetImageView()const = 0 ;
	virtual VkSampler GetSampler() const = 0;

};

class FTexture2D : public FTextureBase
{

public:
	explicit FTexture2D();
	virtual ~FTexture2D();

	static void StaticInit();

	static TSharedPtr <FTexture2D> White();

public:
	template <typename TextureType = FTexture2D>
	static TSharedPtr <TextureType>
	    CreateTexture(const FTextureCreateParams & Params)
	{
		auto Result = std::make_shared<TextureType>();
		Result->Initialize(Params);
		return Result;
	}

	virtual void Initialize(const FTextureCreateParams & Params);

	VkImageView GetImageView() const override
	{
		return ImageView;
	}

	VkSampler GetSampler() const override
	{
		return Sampler;
	}
	VkImage GetImage() const
	{
		return ImageHandle;
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
	FSampler(VkFilter InFilter);
	void Initalize();
	virtual ~FSampler();

	VkSampler Sampler;
	VkFilter Filter = VK_FILTER_LINEAR;
};


#endif //TEXTURE_H
