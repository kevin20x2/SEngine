//
// Created by 郑文凯 on 2024/10/2.
//

#ifndef DEPTHTEXTURE_H
#define DEPTHTEXTURE_H


#include "Texture2D.h"
class FDepthTexture : public FTexture2D
{
public:

	virtual void Initialize(const FTextureCreateParams & Params) override;

};


#endif //DEPTHTEXTURE_H