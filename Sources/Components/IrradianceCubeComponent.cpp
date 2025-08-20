//
// Created by vinkzheng on 2025/8/20.
//

#include "IrradianceCubeComponent.h"

#include "Rendering/Material.h"
#include "Rendering/Renderer.h"
#include "Rendering/RenderingUtils.h"
#include "Rendering/RenderTargetCube.h"
#include "Rendering/Shader.h"
#include "RHI/RHIUtils.h"
#include "Systems/ShaderManager/ShaderManager.h"

void SIrradianceCubeComponent::Init()
{
    auto IrradianceFilterShader = SShaderManager::GetShaderFromName("IrradianceCube");

    Material = SNew<SMaterialInterface>(IrradianceFilterShader->AsShared());
    Material->SetbNeedViewData(false);

    auto Renderer = GEngine->GetModuleByClass<SRenderer>();
    if(CubeRT)
    {
        Material->Initialize(Renderer->GetDescriptorPool()->Pool,CubeRT->GetRenderPass());
    }
}

void SIrradianceCubeComponent::GenerateIrradianceCubeMap()
{

    VkCommandBuffer CommandBuffer = FRHIUtils::BeginOneTimeCommandBuffer();
    if(CubeRT)
    {

        Material->SetTextureCube("CubemapTexture",CubeRT->GetCubeTexture());
        Material->SetScalar("DeltaPhi", 2.0f * FMath::PI / 180.0f );
        Material->SetScalar("DeltaTheta", 0.5f * FMath::PI / 64.0f);

        auto MipLevel = CubeRT->GetCubeTexture()->GetMipLevels();
        for(int32 MipIdx = 1; MipIdx < MipLevel  ; ++MipIdx)
        {
            for(int32 FaceIdx = 0 ; FaceIdx < 6 ; ++ FaceIdx)
            {
                Material->SetInt("CubeFace",FaceIdx);
                Material->SetInt("MipIndex",MipIdx);
                Material->SetInt("NumMips",MipLevel);

                Material->SyncToCommandBuffer(CommandBuffer);


                CubeRT->BeginRenderTargetGroup(CommandBuffer, MipIdx,FaceIdx);

                FRenderingUtils::DrawScreenTriangle(CommandBuffer,Material.get());

                CubeRT->EndRenderTargetGroup(CommandBuffer);

            }
        }
    }
    FRHIUtils::EndOneTimeCommandBuffer(CommandBuffer);
}
