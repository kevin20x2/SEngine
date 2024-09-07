//
// Created by kevin on 2024/8/11.
//

#include "CommandBuffer.h"

#include "RHI.h"
#include "RHIUtils.h"
#include "volk.h"

FCommandBufferPool::FCommandBufferPool()
{
    FQueueFamilyIndices QueueFamilyIndices =
         FRHIUtils::FindQueueFamilies(*GRHI->GetPhysicalDevice(),
             *GRHI->GetCurSurface() );

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = QueueFamilyIndices.GraphicsFamily.value();
	VK_CHECK(vkCreateCommandPool(
		*GRHI->GetDevice()
		, &poolInfo, nullptr, &Pool));
}

FCommandBuffers::FCommandBuffers(uint32 Size,FCommandBufferPool* InPool)
{
	Pool = InPool;
	Buffers.resize(Size);
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = Pool->Pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = Size;

	VK_CHECK(vkAllocateCommandBuffers(
		*GRHI->GetDevice()
		, &allocInfo, Buffers.data()));

}

void FCommandBuffers::FreeCommandBuffer()
{
	vkFreeCommandBuffers(*GRHI->GetDevice(),Pool->Pool,Buffers.size(),Buffers.data());
}
