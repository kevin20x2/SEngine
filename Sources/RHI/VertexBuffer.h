//
// Created by kevin on 2024/8/11.
//

#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H
#include <vulkan/vulkan_core.h>
#include "Core/Core.h"

struct FVertexBufferCreateInfo
{
    uint32 Size;
    float * Data;
};


class FVertexBuffer
{
    FVertexBuffer(const FVertexBufferCreateInfo & Info);

    VkBuffer Buffer{};
    VkDeviceMemory DeviceMemory{};
};

#endif //VERTEXBUFFER_H
