//
// Created by vinkzheng on 2024/10/21.
//

#ifndef IMGUIPORT_H
#define IMGUIPORT_H
#include <backends/imgui_impl_vulkan.h>

#include "GLFW/glfw3.h"


class FImGUIPort
{

public:
    void InitWindow( GLFWwindow* InGLFWwindow   );

    void ShutDown();

    void Render();
    protected:

    ImGui_ImplVulkanH_Window window;

    bool show_demo_window = true;
    bool show_another_window = false;
};



#endif //IMGUIPORT_H
