//
// Created by kevin on 2024/8/17.
//

#include "Mesh.h"

FStaticMesh::FStaticMesh(const TArray<FVector>& InVertices, const TArray<uint16>& InIndexes) :
    Vertices(InVertices), Indexes(InIndexes)
{
}
