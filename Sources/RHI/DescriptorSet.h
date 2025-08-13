//
// Created by kevin on 2024/8/11.
//

#ifndef DESCRIPTORSET_H
#define DESCRIPTORSET_H
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "UniformBuffer.h"
#include "Core/BaseTypes.h"
#include "Texture2D.h"


class FDescriptorSets
{
public:
    static FDescriptorSets * Create(
        const TArray<VkDescriptorSetLayout> & Layouts,
        const FDescriptorPool & Pool,
        const TArray<FUniformBuffer *> & Buffers,
		TSharedPtr<FTexture2D> Texture
        );
    const TArray <VkDescriptorSet> & GetHandle()
    {
        return DescriptorSets;
    }
private:
    TArray <VkDescriptorSet > DescriptorSets;
};



#endif //DESCRIPTORSET_H
