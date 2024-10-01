//
// Created by 郑文凯 on 2024/9/29.
//

#include "CImgTextureLoader.h"
//#define cimg_use_jpeg
#define cimg_use_png
#include "CImg.h"
#include "spdlog/spdlog.h"

using namespace cimg_library;
TSharedPtr<FTexture> FCImgTextureLoader::LoadTexture(const FString &Path)
{
	CImg<uint8> Src(Path.c_str());
	int32 Height = Src.height();
	int32 Width = Src.width();
	uint32 BufferSize = sizeof(uint32) * Height * Width;

	TArray <uint8> ReorderedColor(BufferSize);
	ReorderedColor.resize(BufferSize);

	uint32 Spectrum =  Src.spectrum();
	for(uint32 i = 0 ; i < Height ; ++ i)
	{
		for(uint32 j = 0 ; j < Width; ++ j)
		{
			ReorderedColor[4 *(i * Width + j) + 0] = Src(i,j,0,0);
			ReorderedColor[4 *(i * Width + j) + 1] = Src(i,j,0,1);
			ReorderedColor[4 *(i * Width + j) + 2] = Src(i,j,0,2);
			ReorderedColor[4 *(i * Width + j) + 3] = Spectrum == 3 ?  255 : Src(i,j,0,3);
		}

	}

	FTextureCreateParams Params =
		{
		  .BufferSize =BufferSize,
		  .Height = (uint32) Height,
		  .Width = (uint32) Width,
		  .BufferPtr = ReorderedColor.data()
		};

	//for(int32 i = 127 ; i < 128 ; ++ i)
	{
	//	for(int32 j = 0 ; j < Width; ++ j)
	}

	auto Texture =
		TSharedPtr<FTexture>(new FTexture(Params));

	return Texture;
}
