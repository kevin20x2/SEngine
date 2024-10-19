//
// Created by kevin on 2024/9/3.
//

#include "Engine.h"
#include "LocalPlayer.h"

FEngine * GEngine = nullptr;

FEngine::FEngine()
{
    GEngine = this;
}

void FEngine::Initialize()
{
    Input = new FInput;
    Renderer = new FRenderer;
    Renderer->Initailize();
    LocalPlayer = SNew<SLocalPlayer>();
    LocalPlayer->GetPlayerController()->InitCameraInput();
}

void FEngine::Tick(float DeltaTime)
{
}
