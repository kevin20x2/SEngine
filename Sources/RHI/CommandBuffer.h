//
// Created by kevin on 2024/8/11.
//

#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H
#include "DescriptorPool.h"

class FCommandBufferPool
{
public:
    FCommandBufferPool();

private:
    VkCommandPool Pool;
    friend class FCommandBuffers;
};

class FCommandBuffers
{
public:
    FCommandBuffers(uint32 Size,FCommandBufferPool * Pool);

    void FreeCommandBuffer();

    VkCommandBuffer GetHandle(int32 Idx)
    {
        return Buffers[Idx];
    }

    TArray <VkCommandBuffer> Buffers;

    FCommandBufferPool * Pool;
};



#endif //COMMANDBUFFER_H
