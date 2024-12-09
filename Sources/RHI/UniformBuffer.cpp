//
// Created by kevin on 2024/8/11.
//

#include "UniformBuffer.h"

#include "RHI.h"
#include "RHIUtils.h"
#include "volk.h"
#include "Core/Log.h"

FUniformBuffer* FUniformBuffer::Create(uint32 InSize)
{
	FUniformBuffer * UniformBuffer = new FUniformBuffer;
	UniformBuffer->Size = InSize;
	UniformBuffer->Init();
	return UniformBuffer;
}

FUniformBuffer::~FUniformBuffer()
{
	vkDestroyBuffer(*GRHI->GetDevice(),Buffer,nullptr);
	vkFreeMemory(*GRHI->GetDevice(), BufferMemory,nullptr );
}

void FUniformBuffer::Init()
{
    VkBufferCreateInfo BufferInfo{  };

	BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferInfo.size = Size;
	BufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	// TODO:???
	BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	auto & Device = *GRHI->GetDevice();

	VK_CHECK(vkCreateBuffer(Device, &BufferInfo, nullptr, &Buffer));

	VkMemoryRequirements MemRequrements;
	vkGetBufferMemoryRequirements(Device, Buffer, &MemRequrements);

	VkMemoryAllocateInfo AllocInfo{};
	AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	AllocInfo.allocationSize = MemRequrements.size;
	AllocInfo.memoryTypeIndex =
		FRHIUtils::FindMemoryType(MemRequrements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
	VK_CHECK(vkAllocateMemory(Device, &AllocInfo, nullptr, &BufferMemory));

	vkBindBufferMemory(Device, Buffer, BufferMemory, 0);

}

void FUniformBuffer::UpdateData(void* InData)
{
	auto Device = *GRHI->GetDevice();
	void * Data;
	vkMapMemory(Device,BufferMemory,0,Size,0,&Data);
	memcpy(Data,InData,Size);
	vkUnmapMemory(Device,BufferMemory);
}

bool FUniformBuffer::UpdateData(uint32 Offset, uint32 InSize, void *InData)
{
	if(Offset + InSize > Size )
	{
		SLogD("Data Out of Range ..");
		return false;
	}
	auto Device = *GRHI->GetDevice();
	void * Data;
	vkMapMemory(Device,BufferMemory,Offset,InSize,0,&Data);
	memcpy(Data,InData,InSize);
	vkUnmapMemory(Device,BufferMemory);
	return true;
}
