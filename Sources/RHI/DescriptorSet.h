//
// Created by kevin on 2024/8/11.
//

#ifndef DESCRIPTORSET_H
#define DESCRIPTORSET_H
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "UniformBuffer.h"
#include "Core/BaseTypes.h"


class FDescriptorSets
{
public:
    static FDescriptorSets * Create(
        const TArray<FDescriptorSetLayout> & Layouts,
        const FDescriptorPool & Pool,
        const TArray<FUniformBuffer> & Buffers
        );
private:
    TArray <VkDescriptorSet > DescriptorSets;
};



#endif //DESCRIPTORSET_H
