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


    FRHIUtils::CreateBuffer(
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            Size,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            Buffer,
            BufferMemory
            );
}

void FUniformBuffer::UpdateData(VkCommandBuffer CommandBuffer,void* InData)
{
    UpdateData(CommandBuffer,0,Size,InData );
}

bool FUniformBuffer::UpdateData(VkCommandBuffer CommandBuffer,uint32 Offset, uint32 InSize, void *InData)
{
	if(Offset + InSize > Size )
	{
		SLogD("Data Out of Range ..");
		return false;
	}

    auto StagingBufferOffset = FUniformGlobalStagingBuffer::GetInstance()->
            CopyToStagingBuffer(
                    CommandBuffer,InSize,(uint8*)InData );

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = StagingBufferOffset;
    copyRegion.dstOffset = Offset;
    copyRegion.size = InSize;
    vkCmdCopyBuffer(
            CommandBuffer,
            FUniformGlobalStagingBuffer::GetInstance()->GetBuffer(),          // 源
            Buffer,          // 目标
            1, &copyRegion
    );

    // 4. 添加内存屏障 (确保复制完成后GPU才能使用)
    VkBufferMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.buffer = Buffer;
    barrier.offset = Offset;
    barrier.size = InSize;

    vkCmdPipelineBarrier(
            CommandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,     // 传输阶段
            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, //
            0,
            0, nullptr,
            1, &barrier,
            0, nullptr
    );

	return true;
}

FUniformGlobalStagingBuffer *FUniformGlobalStagingBuffer::GetInstance()
{
    static FUniformGlobalStagingBuffer * LocalInstance = nullptr;
    if(LocalInstance == nullptr)
    {
        LocalInstance = new FUniformGlobalStagingBuffer();
    }

    return LocalInstance;
}

FUniformGlobalStagingBuffer::FUniformGlobalStagingBuffer()
{

    FRHIUtils::CreateBuffer(
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            4*1024 * 1024,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            StagingBuffer,StagingMemory
    );


    VkPhysicalDeviceProperties Props;
    vkGetPhysicalDeviceProperties(
    *GRHI->GetPhysicalDevice(),&Props);
    PaddingSize =  Props.limits.nonCoherentAtomSize;
    SLogD("Padding Size {}",PaddingSize);

}

uint32 FUniformGlobalStagingBuffer::CopyToStagingBuffer(VkCommandBuffer CommandBuffer,
                                                        uint32 BufferSize, uint8 *InBufferData)
{
    uint32 BaseOffset =  CurrentOffset;

    auto MapBufferSize = ( (BufferSize + PaddingSize -1 ) / PaddingSize )*PaddingSize;

    CurrentOffset += MapBufferSize;
    auto Device = *GRHI->GetDevice();
    void * Data;
    vkMapMemory(Device,StagingMemory,BaseOffset,MapBufferSize,0,&Data);
    memcpy(((uint8 *)Data) ,InBufferData,BufferSize);

    VkMappedMemoryRange Range = {
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .memory = StagingMemory,
            .offset = BaseOffset,
            .size = MapBufferSize
    };
    vkFlushMappedMemoryRanges(Device,1,&Range);
    vkUnmapMemory(Device,StagingMemory);

    VkBufferMemoryBarrier bufferBarrier = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .srcAccessMask = VK_ACCESS_HOST_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
            .buffer = StagingBuffer,
            .offset = BaseOffset,
            .size = MapBufferSize
    };
    vkCmdPipelineBarrier(
            CommandBuffer,
            VK_PIPELINE_STAGE_HOST_BIT, // 等待主机写入完成
            VK_PIPELINE_STAGE_TRANSFER_BIT, // 开始传输阶段
            0,
            0, nullptr,
            1, &bufferBarrier,
            0, nullptr
    );

    return BaseOffset;
}
