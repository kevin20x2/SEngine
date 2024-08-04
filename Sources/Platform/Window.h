//
// Created by kevin on 2024/8/4.
//

#pragma once
#include <cstdint>
#include <vulkan/vulkan_core.h>

struct GLFWwindow;

class FWindow
{
public:
    explicit FWindow(int32_t InWidth, int32_t InHeight);

    virtual ~FWindow() ;

    virtual void Init();

    virtual bool ShouldClose();

    virtual void LoopFrame();

    virtual VkSurfaceKHR CreateSurface(VkInstance * Instance) ;

    GLFWwindow * GetHandle() const
    {
        return Handle;
    }

protected:

    GLFWwindow * Handle = nullptr;
    int32_t Width = 0 ;
    int32_t Height = 0 ;
};


