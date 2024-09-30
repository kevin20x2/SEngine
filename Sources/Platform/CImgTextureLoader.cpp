//
// Created by 郑文凯 on 2024/9/29.
//

#include "CImgTextureLoader.h"
//#define cimg_use_jpeg
#define cimg_use_png
#include "CImg.h"

using namespace cimg_library;
TSharedPtr<FTexture> FCImgTextureLoader::LoadTexture(const FString &Path)
{
	CImg<uint32> Src(Path.c_str());
	int32 Height = Src.height();
	int32 Width = Src.width();

	FTextureCreateParams Params =
		{
		  .BufferSize =static_cast<uint32>(sizeof(uint32) * Height * Width ),
		  .Height = (uint32) Height,
		  .Width = (uint32) Width,
		  .BufferPtr = Src.data()
		};

	auto Texture =
		TSharedPtr<FTexture>(new FTexture(Params));

	return Texture;
}
