//
// Created by kevin on 2024/9/3.
//

#include "Engine.h"

#include "EngineModuleBase.h"
#include "LocalPlayer.h"
#include "GUI/ImGUIPort.h"
#include "Platform/Window.h"
#include "AssetManager/AssetManager.h"
#include "Systems/ShaderManager/ShaderManager.h"
#include <chrono>

#include "Core/Log.h"

using namespace std::chrono;

FEngine * GEngine = nullptr;

static high_resolution_clock::time_point EngineInitTime;

FEngine::FEngine()
{
    GEngine = this;
}

void FEngine::Initialize()
{

    EngineInitTime = high_resolution_clock::now();

    Modules.Add(SNew<SShaderManager>());

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

    OnInitialize();
    OnPostInit();
}

void FEngine::OnInitialize()
{
    for(auto Module : Modules)
    {
        if(Module)
        {
            Module->OnInitialize();
        }
    }
}

void FEngine::OnPostInit()
{
    for(auto Module : Modules)
    {
        if(Module)
        {
            Module->OnPostInit();
        }
    }
}

void FEngine::UpdateTime()
{
    high_resolution_clock::time_point Current = high_resolution_clock::now();
    duration<double ,std::micro> TimeElapsed =  Current - EngineInitTime;
    float CurrentTime = (TimeElapsed.count() / 1000000.0f );

    float LastTime = CurrentTimeInSeconds;
    DeltaTime = CurrentTime - LastTime;
    CurrentTimeInSeconds = CurrentTime;
    //SLogD("Update Time: {} DeltaTime : {}", CurrentTime,DeltaTime);
}

void FEngine::Tick()
{
    for(auto Module : Modules)
    {
        if(Module->IsTickable())
        {
            Module->Tick(DeltaTime);
        }
    }
}
