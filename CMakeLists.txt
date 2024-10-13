
cmake_minimum_required(VERSION 3.10)


project(SEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(ThirdParty)

set(EngineSources
        Sources/Application.cpp
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
        Sources/RHI/ShaderProgram.cpp
        Sources/RHI/ShaderProgram.h
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
        Sources/Platform/MeshImporter.cpp
        Sources/Platform/MeshImporter.h
        Sources/Platform/FbxMeshImporter.cpp
        Sources/Platform/FbxMeshImporter.h
        Sources/Platform/FbxCommon/Common.cxx
        Sources/Platform/Path.cpp
        Sources/Platform/Path.h
        Sources/Platform/TextureLoader.cpp
        Sources/Platform/TextureLoader.h
        Sources/RHI/Texture.cpp
        Sources/RHI/Texture.h
        Sources/Platform/CImgTextureLoader.cpp
        Sources/Platform/CImgTextureLoader.h
        Sources/Rendering/Shader.cpp
        Sources/Rendering/Shader.h
        Sources/AssetManager/AssetManager.cpp
        Sources/AssetManager/AssetManager.h
        Sources/Rendering/Material.cpp
        Sources/Rendering/Material.h
        Sources/Rendering/MaterialParameter.cpp
        Sources/Rendering/MaterialParameter.h
        Sources/RHI/DepthTexture.cpp
        Sources/RHI/DepthTexture.h
        Sources/Core/Array.h
        Sources/Rendering/SceneView.cpp
        Sources/Rendering/SceneView.h
        Sources/Rendering/PrimitiveRenderData.cpp
        Sources/Rendering/PrimitiveRenderData.h
        Sources/Rendering/LightRenderData.cpp
        Sources/Rendering/LightRenderData.h

)

set(Spirv_ReflectSource
        ThirdParty/SPIRV-Reflect/spirv_reflect.cpp
)

add_executable(SEngine
        ${Spirv_ReflectSource}
        ${EngineSources}
        Sources/CoreObjects/Controller.cpp
        Sources/CoreObjects/Controller.h
        Sources/CoreObjects/CameraManager.cpp
        Sources/CoreObjects/CameraManager.h
        Sources/CoreObjects/LocalPlayer.cpp
        Sources/CoreObjects/LocalPlayer.h

)

target_compile_definitions(SEngine PUBLIC VK_NO_PROTOTYPES GLM_FORCE_LEFT_HANDED)
target_compile_definitions(SEngine PUBLIC ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(SEngine PUBLIC cimg_display=0)

if(WIN32)
    set(FBX_ROOT_PATH "C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.2")
elseif(APPLE)
    set(FBX_ROOT_PATH "/Applications/Autodesk/FBX SDK/2020.3.2")
endif()

set_property(TARGET SEngine PROPERTY CXX_STANDARD 20)
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
        ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/libpng-1.6.44
        ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/CImg
        ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SPIRV-Reflect
        ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/NanoRtti/Include
 )
if(WIN32)
target_link_directories(SEngine PUBLIC
        ${FBX_ROOT_PATH}/lib/vs2019/x64/debug )
elseif(APPLE)
    target_link_directories(SEngine PUBLIC
       ${FBX_ROOT_PATH}/lib/clang/debug)
endif()

if(WIN32)
 target_link_libraries(SEngine PUBLIC volk glslang SPIRV glslang-default-resource-limits glfw libfbxsdk png_static)
elseif(APPLE)
    target_link_libraries(SEngine PUBLIC c++ c++abi volk glslang SPIRV glslang-default-resource-limits glfw fbxsdk png_static)
endif()