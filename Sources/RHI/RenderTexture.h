//
// Created by kevin on 2024/12/6.
//

#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H
#include "Texture2D.h"


class FRenderTexture : public FTexture2D
{
public:
    virtual void Initialize(const FTextureCreateParams& Params) override;
};


#endif //RENDERTEXTURE_H
