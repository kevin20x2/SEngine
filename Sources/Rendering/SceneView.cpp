//
// Created by kevin on 2024/10/6.
//

#include "SceneView.h"

#include "CoreObjects/CameraComponent.h"
#include "RHI/RHI.h"

FSceneView::FSceneView()
{
    Initialize();
}

void FSceneView::UpdateViewData(SCameraComponent* InCameraComp)
{
    auto ViewMatrix = InCameraComp->GetViewMatrix();
    auto ProjectionMatrix  = InCameraComp->GetProjectinMatrix();
    auto ViewPos = InCameraComp->GetWorldLocation();

    ViewData.ViewProjectionMat = ViewMatrix * ProjectionMatrix;
    ViewData.ViewOrigin = ViewPos;
}

void FSceneView::SyncData(VkCommandBuffer CommandBuffer, uint32 CurrentFrame)
{
    UniformBuffers[CurrentFrame]->UpdateData(CommandBuffer,&ViewData);
}

void FSceneView::Initialize()
{
    UniformBuffers.resize(GRHI->GetMaxFrameInFlight());
    for(auto & UniformBuffer : UniformBuffers)
    {
        UniformBuffer = TSharedPtr<FUniformBuffer>(
            FUniformBuffer::Create(sizeof(ViewData)) );
    }
}
