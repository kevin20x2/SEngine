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

	virtual ~FUniformBuffer();

	VkBuffer GetBuffer() const
	{
		return Buffer;
	}
    void Init();
    void UpdateData(void * InData);
	bool UpdateData(uint32 Offset,uint32 Size ,void * InData );

private:
    uint32 Size;
    VkBuffer Buffer;
    VkDeviceMemory BufferMemory;
    friend class FDescriptorSets;
};



#endif //UNIFORMBUFFER_H
