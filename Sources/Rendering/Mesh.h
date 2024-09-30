//
// Created by kevin on 2024/8/17.
//

#ifndef MESH_H
#define MESH_H
#include "Core/BaseTypes.h"
#include "Maths/Math.h"
#include "RHI/GraphicsPipeline.h"
#include "RHI/VertexBuffer.h"


enum class EMeshVertexElementMask
{
    None = 0 ,
    Position = 0x1,
    Color = 0x2,
    Normal = 0x4,
	TexCoord0 = 0x8
};
S_DEFINE_ENUM_FLAGS(EMeshVertexElementMask)

class FStaticMesh
{
public :
    explicit FStaticMesh(const TArray <FVector> & InVertices, const TArray <uint16> & Indexes );


    static VkPipelineVertexInputStateCreateInfo GenerateVertexInputStateCreateInfo(EMeshVertexElementMask Mask);

    uint32 GetStride() const;

    uint32 GetVBBufferSize() const;

    TSharedPtr<FVertexBuffer> CreateVertexBuffer();

    TArray <FVector> Positions;
    TArray <uint16> Indexes;
    TArray <FVector4> Colors;
    TArray <FVector4> Normals;
	TArray <FVector2> TexCoord0;

    EMeshVertexElementMask ElementMask = EMeshVertexElementMask::Position;
};



#endif //MESH_H
