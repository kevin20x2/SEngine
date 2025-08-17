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

    static TSharedPtr <FRenderTargetCube > Create(uint32 InWidth , uint32 InHeight, uint8 * InData);

    void Initialize(uint32 Width,uint32 Height,uint8 * InData);

    void BeginRenderTargetGroup(VkCommandBuffer CommandBuffer,int32 MipIdx,int32 FaceIdx);

    void EndRenderTargetGroup(VkCommandBuffer CommandBuffer);

    TSharedPtr<FTextureCubeRHI>  GetCubeTexture() const
    {
        return CubeTexture;
    }

    FRenderPass * GetRenderPass()const
    {
        return RenderPass.get();
    }

protected:
    uint32 Width;
    uint32 Height;

    TSharedPtr <FTextureCubeRHI> CubeTexture;
    // Mip, Face
    TArray <TArray <VkImageView> > CubeRT2DViews;

    TArray <TSharedPtr <FFrameBuffer> > FrameBuffers;

    TSharedPtr <FRenderPass> RenderPass;
};


#endif //SENGINE_RENDERTARGETCUBE_H
