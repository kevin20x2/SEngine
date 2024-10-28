//
// Created by kevin on 2024/9/3.
//

#include "Engine.h"

#include "EngineModuleBase.h"
#include "LocalPlayer.h"
#include "GUI/ImGUIPort.h"
#include "Platform/Window.h"
#include "AssetManager/AssetManager.h"

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
    ImGUIPort = SNew<FImGUIPort>();
    if(Window)
    {
        ImGUIPort->InitWindow(Window->GetHandle());
    }

    auto AssetManager = SNew<SAssetManager>();
    Modules.Add(AssetManager);
}

void FEngine::Tick(float DeltaTime)
{
    for(auto Module : Modules)
    {
        if(Module->IsTickable())
        {
            Module->Tick(DeltaTime);
        }
    }
}
