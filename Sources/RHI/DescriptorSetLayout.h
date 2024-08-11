//
// Created by kevin on 2024/8/11.
//

#ifndef DESCRIPTORSETLAYOUT_H
#define DESCRIPTORSETLAYOUT_H
#include <vulkan/vulkan_core.h>


class FDescriptorSetLayout
{
public:
    VkDescriptorSetLayout Layout;
};


class FUniformBufferDescriptorSetLayout : public FDescriptorSetLayout
{
public:
    static FUniformBufferDescriptorSetLayout * Create();
private:
    void Init();
};


#endif //DESCRIPTORSETLAYOUT_H
