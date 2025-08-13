//
// Created by 郑文凯 on 2024/9/29.
//

#ifndef CIMGTEXTURELOADER_H
#define CIMGTEXTURELOADER_H


#include "TextureLoader.h"
class FTextureCubeRHI;

class FCImgTextureLoader : public FTextureLoader
{
public:
	virtual TSharedPtr <FTexture2D> LoadTexture(const FString & Path) override;

	virtual TSharedPtr <FTextureCubeRHI> LoadSingleCubeTexture(const FString & Path) override;
};


#endif //CIMGTEXTURELOADER_H
