//
// Created by 郑文凯 on 2024/9/29.
//

#include "CImgTextureLoader.h"
//#define cimg_use_jpeg
#define cimg_use_png
#include "CImg.h"
#include "Core/Log.h"
#include "Maths/Math.h"
#include "RHI/TextureCube.h"
#include "spdlog/spdlog.h"

using namespace cimg_library;
TSharedPtr<FTexture2D> FCImgTextureLoader::LoadTexture(const FString &Path)
{
	CImg<uint8> Src(Path.c_str());
	int32 Height = Src.height();
	int32 Width = Src.width();
	uint32 BufferSize = sizeof(uint32) * Height * Width;

	TArray <uint8> ReorderedColor(BufferSize);
	ReorderedColor.resize(BufferSize);

	uint32 Spectrum =  Src.spectrum();
	for(int32 i = 0 ; i < Height ; ++ i)
	{
		for(int32 j = 0 ; j < Width; ++ j)
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


	auto Texture =
		FTexture2D::CreateTexture(Params);

	return Texture;
}

TSharedPtr<FTextureCubeRHI> FCImgTextureLoader::LoadSingleCubeTexture(const FString &Path)
{
	CImg<uint8> Src(Path.c_str());
	int32 OriginHeight = Src.height();
	int32 OriginWidth = Src.width();

	if(OriginHeight/3 != OriginWidth / 4 )
	{
		SLogW(TEXT("Invalid CubeMap Size"));
		return nullptr;
	}
	int32 CubeFaceSize = OriginHeight/3;
	FVector2 FixedUVSize = {1.0f/4 ,1.0f/3};
	uint32 Spectrum =  Src.spectrum();

	TArray <FVector2> UVRanges = {
		{1.0f/4,1.0f/3.0f }, // front
		{3.0f/4,1.0f/3}, //back
		{2.0f/4,1.0f/3}, //right
		{0,1.0f/3}, // left
		{1.0f/4,0.0f }, // top
		{1.0f/4,2.0f/3.0f }, // down
	};
	uint32 BufferSize = sizeof(uint32) * CubeFaceSize * CubeFaceSize * 6;
	TArray<uint8 > ColorBuffer(BufferSize);
	for(int32 FaceIdx = 0 ;FaceIdx < 6; ++ FaceIdx)
	{
		auto WidthBegin = static_cast<int32>( OriginWidth *  UVRanges[FaceIdx].x);
		auto HeightBegin =static_cast<int32>( OriginHeight *  UVRanges[FaceIdx ].y);
		int32 BaseFaceOffset =FaceIdx *4*( CubeFaceSize * CubeFaceSize) ;
		for(int32 h = 0; h <  CubeFaceSize ; ++ h )
		{
			for(int32 w= 0; w < CubeFaceSize ; ++ w)
			{

				ColorBuffer[BaseFaceOffset + 4*(( h * CubeFaceSize ) + w ) + 0 ] =  Src(w+WidthBegin,h+HeightBegin,0,0);
				ColorBuffer[BaseFaceOffset + 4*(( h * CubeFaceSize ) + w ) + 1 ] = Src(w+WidthBegin,h+HeightBegin,0,1);
				ColorBuffer[BaseFaceOffset + 4*(( h * CubeFaceSize ) + w ) + 2 ] = Src(w+WidthBegin,h+HeightBegin,0,2);
				ColorBuffer[BaseFaceOffset + 4*(( h * CubeFaceSize ) + w ) + 4 ] =
					Spectrum == 3 ?  255 : Src(w+WidthBegin,h+HeightBegin,0,0);

			}
		}
	}

	FTextureCubeCreateParams CubeCreateParams {
		.Width = (uint32)CubeFaceSize,
		.Height = (uint32)CubeFaceSize,
		.Format = VK_FORMAT_R8G8B8A8_SRGB,
		.Data = ColorBuffer.data(),
	};
	auto CubeTexture = FTextureCubeRHI::Create(CubeCreateParams);
	return CubeTexture ;
}
