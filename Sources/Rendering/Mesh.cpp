//
// Created by kevin on 2024/8/17.
//

#include "Mesh.h"

FStaticMesh::FStaticMesh(const TArray<FVector>& InVertices, const TArray<uint16>& InIndexes) :
    Positions(InVertices), Indexes(InIndexes)
{
}

VkPipelineVertexInputStateCreateInfo FStaticMesh::GenerateVertexInputStateCreateInfo(EMeshVertexElementMask Mask)
{

	VkPipelineVertexInputStateCreateInfo VertexInputInfo{};

	int32 DescriptionCount = 1;
	int32 Stride = sizeof(FVector);
	int32 Offset = 0 ;
	int32 Location = 0;


	static TArray <VkVertexInputAttributeDescription>  Descriptions;
	//auto & Descriptions= * DescriptionsPtr;
	Descriptions.clear();
	auto & PositionDesc =  Descriptions.emplace_back();

	//VkVertexInputAttributeDescription VertexAttributeDescription;
	PositionDesc.binding = 0;
	PositionDesc.location = 0;
	PositionDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
	PositionDesc.offset = Offset;
	Offset += sizeof(FVector);


	if(EnumHasAnyFlags( Mask , EMeshVertexElementMask::Color))
	{
		DescriptionCount++;
		Stride += sizeof(FVector4);
		auto & ColorDesc = Descriptions.emplace_back();
		ColorDesc.binding = 0 ;
		ColorDesc.location = 1;
		ColorDesc.offset = Offset;

		Offset += sizeof(FVector4);
	}

	if(EnumHasAnyFlags(Mask ,EMeshVertexElementMask::Normal))
	{
		DescriptionCount++;
		Stride += sizeof(FVector4);
		auto & NormalDesc = Descriptions.emplace_back();
		NormalDesc.binding = 0 ;
		NormalDesc.location = 2;
		NormalDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		NormalDesc.offset = Offset;

		Offset += sizeof(FVector4);

	}

	if(EnumHasAnyFlags(Mask, EMeshVertexElementMask::TexCoord0))
	{
		DescriptionCount ++ ;
		Stride += sizeof(FVector2);
		auto & UvDesc = Descriptions.emplace_back();
		UvDesc.binding = 0;
		UvDesc.location = 3;
		UvDesc.format = VK_FORMAT_R32G32_SFLOAT;
		UvDesc.offset = Offset;
		Offset += sizeof(FVector2);
	}


	static VkVertexInputBindingDescription VertexBindingDescription;
	VertexBindingDescription.binding = 0;
	VertexBindingDescription.stride = Stride;
	VertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


	VertexInputInfo.sType =
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VertexInputInfo.vertexBindingDescriptionCount = 1;
	VertexInputInfo.pVertexBindingDescriptions = &VertexBindingDescription;
	VertexInputInfo.vertexAttributeDescriptionCount =  Descriptions.size() ;
	VertexInputInfo.pVertexAttributeDescriptions = Descriptions.data();


	return VertexInputInfo;
}

uint32 FStaticMesh::GetStride() const
{
	uint32 Stride = 0;
	if(EnumHasAnyFlags(ElementMask , EMeshVertexElementMask::Position))
	{
		Stride += sizeof(FVector);
	}
	if(EnumHasAnyFlags(ElementMask, EMeshVertexElementMask::Color))
	{
		Stride += sizeof(FVector4);
	}
	if(EnumHasAnyFlags(ElementMask, EMeshVertexElementMask::Normal))
	{
		Stride += sizeof(FVector4);
	}
	if(EnumHasAnyFlags(ElementMask , EMeshVertexElementMask::TexCoord0))
	{
		Stride += sizeof(FVector2);
	}

	return Stride;
}

uint32 FStaticMesh::GetVBBufferSize() const
{
	auto Stride = GetStride();
	return Stride * Positions.size();
}

TSharedPtr<FVertexBuffer> FStaticMesh::CreateVertexBuffer()
{
	uint32 BufferSize = GetVBBufferSize();
	TArray <char> Buffer;
	Buffer.resize(BufferSize);
	char * pBuffer = Buffer.data();

	for(uint32 Idx = 0 ; Idx < Positions.size();  ++Idx)
	{
		memcpy(pBuffer, &Positions[Idx],sizeof(FVector));
		pBuffer += sizeof(FVector);

		if(EnumHasAnyFlags(ElementMask, EMeshVertexElementMask::Color))
		{
			memcpy(pBuffer, & Colors[Idx],sizeof(FVector4));
			pBuffer += sizeof(FVector4);
		}
		if(EnumHasAnyFlags(ElementMask, EMeshVertexElementMask::Normal))
		{
			memcpy(pBuffer, & Normals[Idx],sizeof(FVector4));
			pBuffer += sizeof(FVector4);
		}
		if(EnumHasAnyFlags(ElementMask,EMeshVertexElementMask::TexCoord0))
		{
			memcpy(pBuffer,&TexCoord0[Idx],sizeof(FVector2));
			pBuffer += sizeof(FVector2);
		}
	}

	auto Result = TSharedPtr<FVertexBuffer>(new FVertexBuffer({BufferSize,(float *)Buffer.data()}));

	return Result;
}
