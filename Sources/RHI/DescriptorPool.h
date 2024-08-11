//
// Created by kevin on 2024/8/11.
//

#ifndef DESCRIPTORPOOL_H
#define DESCRIPTORPOOL_H
#include "DescriptorSetLayout.h"
#include "Core/BaseTypes.h"


class FDescriptorPool
{
public:
    VkDescriptorPool Pool;
};


class FUniformBufferDescriptorPool : public FDescriptorPool
{

public:
    static FUniformBufferDescriptorPool* Create(uint32 DescriptorCount, uint32 MaxSetNum);
private:
};


#endif //DESCRIPTORPOOL_H
