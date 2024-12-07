//
// Created by kevin on 2024/12/7.
//

#ifndef RENDERTARGETGROUP_H
#define RENDERTARGETGROUP_H
#include "DepthTexture.h"
#include "FrameBuffer.h"
#include "RenderPass.h"
#include "RenderTexture.h"
#include "Maths/Math.h"


struct FRenderTargetGroupCreateParams
{
    uint32 Width;
    uint32 Height;
};

class FRenderTargetGroup
{
public:

    void InitializeBySwapChain(FSwapChain * InSwapChain);

    void Initialize(FRenderTargetGroupCreateParams & Params);

    void BeginRenderTargetGroup(VkCommandBuffer CommandBuffer,uint32 ImageIdx,FVector4 InClearColor);

    void EndRenderTargetGroup(VkCommandBuffer CommandBuffer);

    FRenderPass * GetRenderPass() const
    {
        return RenderPass.get();
    }

protected:
    TSharedPtr <FRenderPass> RenderPass;

    TArray <
    TSharedPtr <FFrameBuffer>> FrameBuffer;

    TArray <TSharedPtr <FRenderTexture> > RenderTexture;

    TArray <TSharedPtr <FDepthTexture >> DepthTexture;

    FRenderTargetGroupCreateParams CreateParams;
};


#endif //RENDERTARGETGROUP_H
