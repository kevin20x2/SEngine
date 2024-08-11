//
// Created by kevin on 2024/8/11.
//
#include "VertexBuffer.h"

#include "RHI.h"
#include "volk.h"


FVertexBuffer::FVertexBuffer(const FVertexBufferCreateInfo& Info)
{
    VkBufferCreateInfo BufferInfo {};

    BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferInfo.size = Info.Size;
    BufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkCreateBuffer(*GRHI->GetDevice(),&BufferInfo,nullptr,&Buffer);

}
