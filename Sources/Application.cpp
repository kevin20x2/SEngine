#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdio>
#include "VkEngine.h"
#include "CoreObjects/Engine.h"
#include "Platform/Window.h"
#include "Rendering/Renderer.h"
#include "RHI/RHI.h"

int main()
{

    //GLFWwindow* Window = glfwCreateWindow(800, 600, "Vulkan WIndow", nullptr, nullptr);
    FWindow * Window = new FWindow(800,600);
    Window->Init();

    InitRHI(Window);

    FEngine * Engine = new FEngine;
    Engine->Initialize();

    //FRenderer * Renderer = new FRenderer();
    //Renderer->Initailize();
    //KzVkEngine * Engine = new KzVkEngine(Window);
    //Engine->InitVulkan();

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
