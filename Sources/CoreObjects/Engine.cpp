//
// Created by kevin on 2024/9/3.
//

#include "Engine.h"

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
}

void FEngine::Tick(float DeltaTime)
{
}
