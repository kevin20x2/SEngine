//
// Created by kevin on 2024/8/17.
//

#ifndef MESH_H
#define MESH_H
#include "Core/BaseTypes.h"
#include "Maths/Vector.h"


class FStaticMesh
{
public :
    explicit FStaticMesh(const TArray <FVector> & InVertices, const TArray <uint16> & Indexes );

    TArray <FVector> Vertices;
    TArray <uint16> Indexes;
};



#endif //MESH_H
