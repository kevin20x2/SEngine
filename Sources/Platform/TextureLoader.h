//
// Created by 郑文凯 on 2024/9/29.
//

#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include "RHI/Texture.h"
#include "Core/BaseTypes.h"

class FTextureLoader
{
	public :

	virtual TSharedPtr < FTexture > LoadTexture(const FString & FilePath ) = 0 ;
};

#endif //TEXTURELOADER_H
