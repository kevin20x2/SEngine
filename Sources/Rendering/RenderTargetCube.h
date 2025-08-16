//
// Created by kevin on 2025/8/16.
//

#ifndef SENGINE_RENDERTARGETCUBE_H
#define SENGINE_RENDERTARGETCUBE_H

#include "RHI/TextureCube.h"
#include "RHI/FrameBuffer.h"

class FRenderTargetCube
{

public:
    void Initialize(uint32 Width,uint32 Height);

protected:
    TSharedPtr <FTextureCubeRHI> CubeTexture;

    // Mip, Face
    TArray <TArray <VkImageView> > CubeRT2DViews;

    TArray <TSharedPtr <FFrameBuffer> > FrameBuffers;

    TSharedPtr <FRenderPass> RenderPass;
};


#endif //SENGINE_RENDERTARGETCUBE_H
