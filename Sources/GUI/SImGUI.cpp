//
// Created by vinkzheng on 2024/10/21.
//

#include "volk.h"
#include "SImGUI.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "Core/Log.h"
#include "CoreObjects/Engine.h"
#include "Rendering/Renderer.h"
#include "RHI/RHI.h"



static void Check_Vk_result(VkResult Err)
{
    if(Err == 0)
        return ;

    SLogE("vulkan  Error : VkResult = {}\n",(int32)Err);
}

void SImGUI::InitWindow(GLFWwindow *InGLFWwindow)
{

    ImGui_ImplVulkanH_Window * Wd = &Window;

    Wd->Surface = *GRHI->GetCurSurface();
    auto QueueFamily = GRHI->GetQueueFamily();

    auto PhysicalDevice = *GRHI->GetPhysicalDevice();
    // Check for WSI support
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, QueueFamily, Wd->Surface, &res);
    if (res != VK_TRUE)
    {
        fprintf(stderr, "Error no WSI support on physical device 0\n");
        exit(-1);
    }

    ImGui_ImplVulkan_LoadFunctions([](const char * FunctionName,void * VulkanInstance )
    {
        return vkGetInstanceProcAddr(*((VkInstance*)VulkanInstance), FunctionName);
    }, GRHI->GetInstance());

    // Select Surface Format
    const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    Wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(*GRHI->GetPhysicalDevice(), Wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

    // Select Present Mode
#ifdef APP_USE_UNLIMITED_FRAME_RATE
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
    Wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(PhysicalDevice, Wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
    //printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

    auto Width = GRHI->GetDisplaySize().width;
    auto Height = GRHI->GetDisplaySize().height;
    // Create SwapChain, RenderPass, Framebuffer, etc.
    //ImGui_ImplVulkanH_CreateOrResizeWindow(*GRHI->GetInstance(), PhysicalDevice, * GRHI->GetDevice(), Wd, QueueFamily, nullptr, Width, Height, GRHI->GetMaxFrameInFlight());


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(InGLFWwindow, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = *GRHI->GetInstance();
    init_info.PhysicalDevice = *GRHI->GetPhysicalDevice();
    init_info.Device = *GRHI->GetDevice();
    init_info.QueueFamily = QueueFamily;
    init_info.Queue = *GRHI->GetGraphicsQueue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = GEngine->GetRenderer()->GetDescriptorPool()->Pool;
    init_info.RenderPass = GEngine->GetRenderer()->GetSwapChainRenderPass()->RenderPass;
    init_info.Subpass = 0;
    init_info.MinImageCount = GRHI->GetMaxFrameInFlight();
    init_info.ImageCount = GRHI->GetMaxFrameInFlight();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = Check_Vk_result;
    ImGui_ImplVulkan_Init(&init_info);


}

void SImGUI::ShutDown()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void SImGUI::OnRender(VkCommandBuffer CommandBuffer)
{
    ImDrawData *draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data , CommandBuffer);
}

void SImGUI::OnRecordGUIData()
{
    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGuiIO & io = ImGui::GetIO();
    ImGui::NewFrame();


    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float *) &clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))
            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);
        // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    //const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

}


