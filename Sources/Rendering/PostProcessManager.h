//
// Created by kevin on 2024/12/7.
//

#ifndef POSTPROCESSMANAGER_H
#define POSTPROCESSMANAGER_H
#include "RHI/IndexBuffer.h"
#include "RHI/VertexBuffer.h"


class FPostProcessManager
{

    void InitRenderResource();

    TSharedPtr <FVertexBuffer> TriVB;
    TSharedPtr <FIndexBuffer> TriIB;
};

#endif //POSTPROCESSMANAGER_H
