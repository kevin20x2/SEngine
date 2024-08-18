//
// Created by kevin on 2024/8/17.
//

#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H
#include "CommandBuffer.h"

struct FIndexBufferCreateInfo
{
    uint32 Size;
    uint16 * Data;
};

class FIndexBuffer
{

public:
    FIndexBuffer(const FIndexBufferCreateInfo & Info);
    VkBuffer GetHandle() {return Buffer;}
protected:
    VkBuffer Buffer {};
    VkDeviceMemory DeviceMemory{};
};



#endif //INDEXBUFFER_H
