//
// Created by vinkzheng on 2025/8/26.
//

#include "BRDFLUTRenderComponent.h"

#include "Rendering/Material.h"
#include "Rendering/Renderer.h"
#include "Rendering/RenderingUtils.h"
#include "Rendering/Shader.h"
#include "RHI/RenderTargetGroup.h"
#include "RHI/RHIUtils.h"
#include "Systems/ShaderManager/ShaderManager.h"


void SBRDFLUTRenderComponent::Init()
{
    auto GenBrdfShader = SShaderManager::GetShaderFromName("GenBRDFLUT");

    Material = SNew <SMaterialInterface>(GenBrdfShader->AsShared());
    Material->SetbNeedViewData(false);

    FRenderTargetGroupCreateParams CreateParams
    {
        .Width = 1024,
        .Height = 1024,
    };

    RTG = TSharedPtr <FRenderTargetGroup>(new FRenderTargetGroup());
    RTG->Initialize(CreateParams);

    auto Renderer = GEngine->GetModuleByClass<SRenderer>();
    Material->Initialize(Renderer->GetDescriptorPool()->Pool,RTG->GetRenderPass());

    Render();
}

void SBRDFLUTRenderComponent::Render()
{
    VkCommandBuffer CommandBuffer = FRHIUtils::BeginOneTimeCommandBuffer();

    RTG->BeginRenderTargetGroup(CommandBuffer,0,FVector4(0,0,0,0));

    FRenderingUtils::DrawScreenTriangle(CommandBuffer,Material.get());

    RTG->EndRenderTargetGroup(CommandBuffer);
}
