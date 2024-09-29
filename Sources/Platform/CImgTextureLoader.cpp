//
// Created by 郑文凯 on 2024/9/29.
//

#include "CImgTextureLoader.h"
#include "CImg.h"

using namespace cimg_library;
TSharedPtr<FTexture> FCImgTextureLoader::LoadTexture(const FString &Path)
{
	CImg<uint32> Src(Path.c_str());
	int32 Height = Src.height();
	int32 Width = Src.width();

	return nullptr;
}
