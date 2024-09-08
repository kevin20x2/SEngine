
cmake_minimum_required(VERSION 3.10)


project(SEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(ThirdParty)

add_executable(SEngine Sources/Application.cpp 
					Sources/VkEngine.cpp 
					Sources/VkEngine.h
        Sources/Platform/Window.cpp
        Sources/Platform/Window.h
        Sources/RHI/VertexBuffer.h
        Sources/Core/BaseTypes.h
        Sources/Core/Core.h
        Sources/RHI/VertexBuffer.cpp
        Sources/RHI/RHI.cpp
        Sources/RHI/RHI.h
        Sources/RHI/RHIUtils.cpp
        Sources/RHI/RHIUtils.h
        Sources/RHI/SwapChain.cpp
        Sources/RHI/SwapChain.h
        Sources/RHI/DescriptorSetLayout.cpp
        Sources/RHI/DescriptorSetLayout.h
        Sources/RHI/UniformBuffer.cpp
        Sources/RHI/UniformBuffer.h
        Sources/RHI/DescriptorPool.cpp
        Sources/RHI/DescriptorPool.h
        Sources/RHI/DescriptorSet.cpp
        Sources/RHI/DescriptorSet.h
        Sources/RHI/RenderPass.cpp
        Sources/RHI/RenderPass.h
        Sources/RHI/Shader.cpp
        Sources/RHI/Shader.h
        Sources/RHI/GraphicsPipeline.cpp
        Sources/RHI/GraphicsPipeline.h
        Sources/Rendering/Renderer.cpp
        Sources/Rendering/Renderer.h
        Sources/RHI/CommandBuffer.cpp
        Sources/RHI/CommandBuffer.h
        Sources/RHI/FrameBuffer.cpp
        Sources/RHI/FrameBuffer.h
        Sources/Maths/Vector2.cpp
        Sources/Maths/Vector2.h
        Sources/Rendering/Mesh.cpp
        Sources/Rendering/Mesh.h
        Sources/Maths/Vector.cpp
        Sources/Maths/Vector.h
        Sources/CoreObjects/SObject.cpp
        Sources/CoreObjects/SObject.h
        Sources/CoreObjects/SCoreComponentBase.cpp
        Sources/CoreObjects/SCoreComponentBase.h
        Sources/CoreObjects/SSceneComponent.cpp
        Sources/CoreObjects/SSceneComponent.h
        Sources/CoreObjects/Actor.cpp
        Sources/CoreObjects/Actor.h
        Sources/CoreObjects/CameraComponent.cpp
        Sources/CoreObjects/CameraComponent.h
        Sources/CoreObjects/PrimitiveComponent.cpp
        Sources/CoreObjects/PrimitiveComponent.h
        Sources/RHI/IndexBuffer.cpp
        Sources/RHI/IndexBuffer.h
        Sources/Maths/Transform.cpp
        Sources/Maths/Transform.h
        Sources/Maths/Quat.cpp
        Sources/Maths/Quat.h
        Sources/Maths/Math.cpp
        Sources/Maths/Math.h
        Sources/Maths/Matrix4.cpp
        Sources/Maths/Matrix4.h
        Sources/Maths/Vector4.cpp
        Sources/Maths/Vector4.h
        Sources/CoreObjects/World.cpp
        Sources/CoreObjects/World.h
        Sources/CoreObjects/Engine.cpp
        Sources/CoreObjects/Engine.h
        Sources/Input/Input.cpp
        Sources/Input/Input.h
        Sources/CoreObjects/Material.cpp
        Sources/CoreObjects/Material.h
        Sources/Platform/MeshImporter.cpp
        Sources/Platform/MeshImporter.h
        Sources/Platform/FbxMeshImporter.cpp
        Sources/Platform/FbxMeshImporter.h
        Sources/Platform/FbxCommon/Common.cxx)

target_compile_definitions(SEngine PUBLIC VK_NO_PROTOTYPES GLM_FORCE_LEFT_HANDED)

set(FBX_ROOT_PATH "C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.2")

set_property(TARGET SEngine PROPERTY CXX_STANDARD 17)
set_property(TARGET SEngine PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET SEngine PROPERTY CXX_EXTENSIONS Off)

target_include_directories(SEngine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Sources
 ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vulkan/include
 ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/include
 ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/volk
        ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm
        ${FBX_ROOT_PATH}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/include
        #${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/eigen-3.4.0
 )

target_link_directories(SEngine PUBLIC
        ${FBX_ROOT_PATH}/lib/vs2019/x64/debug
)


 target_link_libraries(SEngine PUBLIC volk glslang SPIRV glslang-default-resource-limits glfw libfbxsdk)