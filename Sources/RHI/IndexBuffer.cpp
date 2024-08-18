//
// Created by kevin on 2024/8/17.
//

#include "IndexBuffer.h"

#include "RHI.h"
#include "RHIUtils.h"
#include "volk.h"

FIndexBuffer::FIndexBuffer(const FIndexBufferCreateInfo& Info)
{
    VkBufferCreateInfo BufferInfo{};


    BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferInfo.size = Info.Size;
    BufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


    auto Device = *GRHI->GetDevice();
    vkCreateBuffer(Device,&BufferInfo,nullptr, &Buffer);

    VkMemoryRequirements MemRequrements;

    vkGetBufferMemoryRequirements(Device,Buffer,&MemRequrements);

    VkMemoryAllocateInfo AllocInfo {};

    AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    AllocInfo.allocationSize = MemRequrements.size;
    AllocInfo.memoryTypeIndex =
        FRHIUtils::FindMemoryType(MemRequrements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
    VK_CHECK(vkAllocateMemory(Device,&AllocInfo,nullptr,&DeviceMemory));

    vkBindBufferMemory(Device,Buffer, DeviceMemory,0);

    if(Info.Data)
    {
        void * Data;
        vkMapMemory(Device,DeviceMemory,0,Info.Size,0, &Data);
        memcpy(Data,Info.Data,Info.Size);
        vkUnmapMemory(Device,DeviceMemory);
    }
}
