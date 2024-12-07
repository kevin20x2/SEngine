//
// Created by kevin on 2024/12/6.
//

#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H
#include "Texture.h"


class FRenderTexture : public FTexture
{
public:
    virtual void Initialize(const FTextureCreateParams& Params) override;
};


#endif //RENDERTEXTURE_H
