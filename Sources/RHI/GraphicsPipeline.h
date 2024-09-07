//
// Created by kevin on 2024/8/11.
//

#ifndef GRAPHICSPIPELINE_H
#define GRAPHICSPIPELINE_H
#include <vulkan/vulkan_core.h>

#include "DescriptorSetLayout.h"
#include "RenderPass.h"
#include "Shader.h"


struct FGrpahicsPipelineCreateInfo
{
    FVertexShaderProgram * VertexShader;
    FPixelShaderProgram * PixelShader;
    FDescriptorSetLayout * DescriptorSetLayout;
    FRenderPass * RenderPass;
};

class FGraphicsPipeline
{
public:

    VkPipeline GetHandle()
    {
        return Pipeline;
    }
    VkPipelineLayout GetLayout()
    {
        return Layout;
    }

    FGraphicsPipeline(FGrpahicsPipelineCreateInfo Info);
protected:
    VkPipeline Pipeline;
    VkPipelineLayout Layout;
};



#endif //GRAPHICSPIPELINE_H
