#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//#include "VkEngine.h"
#include "CoreObjects/Engine.h"
#include "Platform/Window.h"
#include "RHI/RHI.h"

int main()
{

    FWindow * Window = new FWindow(1440,720);
    Window->Init();

    InitRHI(Window);

    FEngine * Engine = new FEngine;
    Engine->Initialize();

    auto Renderer =Engine->GetRenderer();


    while (!Window->ShouldClose())
    {
        Window->LoopFrame();
        //Engine->Render();
        Renderer->Render();
    }

    delete Window;

    return 0;
}
