//
// Created by kevin on 2024/8/11.
//

#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H
#include "VertexBuffer.h"


class FUniformBuffer
{
public:
    static FUniformBuffer * Create(uint32 InSize);

    void Init();
    void UpdateData(void * InData);
private:
    uint32 Size;
    VkBuffer Buffer;
    VkDeviceMemory BufferMemory;
    friend class FDescriptorSets;
};



#endif //UNIFORMBUFFER_H
