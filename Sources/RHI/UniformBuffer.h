//
// Created by kevin on 2024/8/11.
//

#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H
#include "VertexBuffer.h"


class FUniformGlobalStagingBuffer
{

public:
    static FUniformGlobalStagingBuffer * GetInstance();

    FUniformGlobalStagingBuffer();

    uint32 CopyToStagingBuffer(VkCommandBuffer CommandBuffer,uint32 BufferSize,uint8 * InBufferData);

    VkBuffer GetBuffer() const
    {
        return StagingBuffer;
    }

    void ResetOffset()
    {
        CurrentOffset = 0;
    }

protected:

    uint32 PaddingSize;

    uint32 CurrentOffset = 0;
    VkBuffer StagingBuffer;
    VkDeviceMemory StagingMemory;

};

class FUniformBuffer
{
public:
    static FUniformBuffer * Create(uint32 InSize);

	virtual ~FUniformBuffer();

	VkBuffer GetBuffer() const
	{
		return Buffer;
	}
    void Init();
    void UpdateData(VkCommandBuffer CommandBuffer ,void * InData);
	bool UpdateData(VkCommandBuffer CommandBuffer ,
                    uint32 Offset,uint32 Size ,void * InData );

private:
    uint32 Size;

    VkBuffer Buffer;
    VkDeviceMemory BufferMemory;
    friend class FDescriptorSets;
};



#endif //UNIFORMBUFFER_H
