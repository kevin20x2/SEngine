//
// Created by kevin on 2025/8/17.
//

#ifndef SENGINE_RENDERINGUTILS_H
#define SENGINE_RENDERINGUTILS_H


#include "Material.h"

class FRenderingUtils
{
public:
    static void DrawScreenTriangle(VkCommandBuffer CommandBuffer,SMaterialInterface * InMaterial);

};


#endif //SENGINE_RENDERINGUTILS_H
