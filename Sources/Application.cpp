#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "CoreObjects/Engine.h"
#include "Platform/Window.h"
#include "RHI/RHI.h"

int main()
{

    auto  Window = SNew <FWindow>(1440,720);
    Window->Init();

    InitRHI(Window.get());

    FEngine * Engine = new FEngine;
    Engine->SetWindow(Window);
    Engine->Initialize();

    auto Renderer = Engine->GetRenderer();

    while (!Window->ShouldClose())
    {
        Window->LoopFrame();
        //Engine->Render();
        Renderer->Render();
    }

    //delete Window;

    return 0;
}
