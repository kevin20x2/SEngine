#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdio>
#include "VkEngine.h"
#include "Platform/Window.h"

int main()
{

    //GLFWwindow* Window = glfwCreateWindow(800, 600, "Vulkan WIndow", nullptr, nullptr);
    FWindow * Window = new FWindow(800,600);
    Window->Init();

    KzVkEngine * Engine = new KzVkEngine(Window);
    Engine->InitVulkan();


    while (!Window->ShouldClose())
    {
        Window->LoopFrame();
        Engine->Render();
    }

    delete Window;

    return 0;
}
