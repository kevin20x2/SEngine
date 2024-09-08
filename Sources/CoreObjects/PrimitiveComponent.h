//
// Created by kevin on 2024/8/17.
//

#ifndef PRIMITIVECOMPONENT_H
#define PRIMITIVECOMPONENT_H
#include "SCoreComponentBase.h"
#include "Rendering/Mesh.h"
#include "RHI/IndexBuffer.h"
#include "RHI/VertexBuffer.h"


class SPrimitiveComponent : public SCoreComponentBase
{

public:
    SPrimitiveComponent(TSharedPtr<SActor > InActor) :
    SCoreComponentBase(InActor) {}
    void SetStaticMesh(TSharedPtr<FStaticMesh> InMesh)
    {
        Mesh = InMesh;
    }
    void CreateRHIResource();
    virtual void OnRecordCommandBuffer(VkCommandBuffer CommandBuffer);
protected:

    TSharedPtr<FStaticMesh > Mesh;



    TSharedPtr<FVertexBuffer> VB;
    TSharedPtr <FIndexBuffer> IB;
};

#endif //PRIMITIVECOMPONENT_H
