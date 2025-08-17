//
// Created by kevin on 2024/12/7.
//

#ifndef POSTPROCESSMANAGER_H
#define POSTPROCESSMANAGER_H
#include "PostProcess.h"
#include "RHI/IndexBuffer.h"
#include "RHI/VertexBuffer.h"


class FPostProcessManager
{
public:
    void BeginPostProcess(uint32 FrameIdx,
        VkCommandBuffer CommandBuffer ,FRenderTargetGroup * InBaseGroup, FRenderTargetGroup * SwapChainGroup );

    void OnPreRecordCommandBuffer(VkCommandBuffer CommandBuffer,
                                  uint32 ImageIndex , FRenderTargetGroup * InBaseGroup ,FRenderTargetGroup * SwapChainGroup);

    void InitRenderResource();
protected:

    TSharedPtr<SMaterialInterface> BlitMaterial;
    TSharedPtr <SMaterialInterface> OutlineMaterial;

    TArray <TSharedPtr <FPostProcess>> PostProcesses;
    TSharedPtr <FVertexBuffer> TriVB;
    TSharedPtr <FIndexBuffer> TriIB;
};

#endif //POSTPROCESSMANAGER_H
