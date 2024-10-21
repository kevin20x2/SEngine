//
// Created by kevin on 2024/8/4.
//

#include "Window.h"

#include <stdexcept>

#include "GLFW/glfw3.h"
#include "Input/Input.h"
#include "Rendering/Renderer.h"



FWindow::FWindow(int32_t InWidth, int32_t InHeight) :
Width(InWidth),Height(InHeight)
{
}

FWindow::~FWindow()
{
    glfwDestroyWindow(Handle);
    glfwTerminate();
}

void FWindow::Init()
{
    if(!glfwInit())
    {
        throw std::runtime_error("init GLFW Error !");
    }


    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    Handle = glfwCreateWindow(Width, Height, "SEngine",nullptr,nullptr);


    if(!Handle)
    {
        throw std::runtime_error("Create Window Failed");
    }

    glfwSetKeyCallback(Handle,RawInputEvent);
    glfwSetScrollCallback(Handle,RawScrollCallback);
    glfwSetFramebufferSizeCallback(Handle,OnRawWindowResize);
    glfwSetCursorPosCallback(Handle,RawCursorPositionCallback);
    glfwSetMouseButtonCallback(Handle,RawMouseButtonCallback);

    glfwSetInputMode(Handle, GLFW_STICKY_KEYS,1);
    glfwSetInputMode(Handle, GLFW_STICKY_MOUSE_BUTTONS,1);

}

bool FWindow::ShouldClose()
{
    return glfwWindowShouldClose(Handle);
}

void FWindow::LoopFrame()
{
    glfwPollEvents();
}

VkSurfaceKHR FWindow::CreateSurface(VkInstance* Instance)
{
    VkSurfaceKHR Surface;
    if(!Instance) return Surface;
    if(glfwCreateWindowSurface(*Instance,Handle, nullptr, &Surface) != VK_SUCCESS)
    {

    }
    return Surface;
}
