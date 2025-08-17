//
// Created by kevin on 2025/8/17.
//

#include "ReflectionCaptureComponent.h"
#include "Systems/ShaderManager/ShaderManager.h"
#include "Rendering/RenderTargetCube.h"
#include "Rendering/Renderer.h"
#include "Rendering/RenderingUtils.h"
#include "RHI/RHIUtils.h"

void SReflectionCaptureComponent::FilterCubeMap()
{
    auto CubeMapFilterShader = SShaderManager::GetShaderFromName("CubemapFilter");
    CubeFilterMaterial = SNew <SMaterialInterface>(CubeMapFilterShader->AsShared());
    CubeFilterMaterial->SetbNeedViewData(false);

    auto CommandBuffer = FRHIUtils::BeginOneTimeCommandBuffer();

    auto Renderer = GEngine->GetModuleByClass<SRenderer>();
    if(CubeRT)
    {
        CubeFilterMaterial->Initialize( Renderer->GetDescriptorPool()->Pool , CubeRT->GetRenderPass());
        auto MipLevel = CubeRT->GetCubeTexture()->GetMipLevels();
        for(int32 MipIdx = 1; MipIdx < MipLevel  ; ++MipIdx)
        {
            for(int32 FaceIdx = 0 ; FaceIdx < 6 ; ++ FaceIdx)
            {
                CubeFilterMaterial->SetInt("CubeFace",FaceIdx);
                CubeFilterMaterial->SetInt("MipIndex",MipIdx);
                CubeFilterMaterial->SetInt("NumMips",MipLevel);


                CubeRT->BeginRenderTargetGroup(CommandBuffer, MipIdx,FaceIdx);

                FRenderingUtils::DrawScreenTriangle(CommandBuffer,CubeFilterMaterial.get());

                CubeRT->EndRenderTargetGroup(CommandBuffer);
            }
        }
    }

    FRHIUtils::EndOneTimeCommandBuffer(CommandBuffer);
}
