#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdio>
#include "VkEngine.h"

int main()
{
    printf("hello world");
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* Window = glfwCreateWindow(800, 600, "Vulkan WIndow", nullptr, nullptr);

    uint32_t ExtensionCnt = 0;



    while (!glfwWindowShouldClose(Window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(Window);

    glfwTerminate();

    return 0;
}