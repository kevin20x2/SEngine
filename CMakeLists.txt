
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
        Sources/RHI/RHIUtils.h)

target_compile_definitions(SEngine PUBLIC VK_NO_PROTOTYPES)

set_property(TARGET SEngine PROPERTY CXX_STANDARD 17)
set_property(TARGET SEngine PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET SEngine PROPERTY CXX_EXTENSIONS Off)

target_include_directories(SEngine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Sources
 ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vulkan/include
 ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/include
 ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/volk
 )

 target_link_libraries(SEngine PUBLIC volk glslang SPIRV glslang-default-resource-limits glfw)