//
// Created by kevin on 2024/8/17.
//

#ifndef PRIMITIVECOMPONENT_H
#define PRIMITIVECOMPONENT_H
#include "SCoreComponentBase.h"
#include "SSceneComponent.h"
#include "Rendering/Mesh.h"
#include "RHI/IndexBuffer.h"
#include "RHI/VertexBuffer.h"
#include "Rendering/Material.h"

class FPrimitiveRenderData;

class SPrimitiveComponent : public SSceneComponent
{

public:
	S_REGISTER_CLASS(SSceneComponent)

    SPrimitiveComponent(TSharedPtr<SActor > InActor) :
    SSceneComponent(InActor) {}
    void SetStaticMesh(TSharedPtr<FStaticMesh> InMesh)
    {
        Mesh = InMesh;
    }
    void CreateRHIResource();
	virtual void OnPreRecordCommandBuffer(uint32 CurrentFrame);

    virtual void OnRecordCommandBuffer(VkCommandBuffer CommandBuffer,uint32 CurrentFrame);

	SMaterialInterface * GetMaterial() const
	{
		return Material.get();
	}

	void SetMaterial(TSharedPtr<SMaterialInterface> InMaterial)
	{
		Material = InMaterial;
	}

	virtual void OnRegister() override;
	virtual void OnUnRegister() override;

protected:

	TSharedPtr <SMaterialInterface> Material;

    TSharedPtr<FStaticMesh > Mesh;
    TSharedPtr<FVertexBuffer> VB;
    TSharedPtr <FIndexBuffer> IB;

	TSharedPtr <FPrimitiveRenderData > RenderData;
};

#endif //PRIMITIVECOMPONENT_H
