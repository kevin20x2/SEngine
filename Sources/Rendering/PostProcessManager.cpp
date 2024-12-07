//
// Created by kevin on 2024/12/7.
//

#include "PostProcessManager.h"

#include "Maths/Math.h"

void FPostProcessManager::InitRenderResource()
{

    TArray <FVector> Positions = {
        FVector(2.0f,0,0),
        FVector(0,0,0),
        FVector(0,2,0)
    };
    TArray <FVector2> Uv =
    {
        FVector2(2,0),
        FVector2(0,0),
        FVector2(0,2)
    };

    const uint32 Stride = sizeof(FVector) + sizeof(FVector2);
    uint32 BufferSize = Positions.size() * Stride;
    TArray<char > Buffer;
    Buffer.resize(BufferSize);
    char * pBuffer = Buffer.data();

    for(uint32 Idx = 0 ; Idx < Positions.size(); ++ Idx)
    {
        memcpy(pBuffer,&Positions[Idx],sizeof(FVector));
        pBuffer  += sizeof(FVector);
        memcpy(pBuffer,&Uv[Idx],sizeof(FVector2));
        pBuffer +=sizeof(FVector2);
    }

    TriVB = TSharedPtr<FVertexBuffer>(new FVertexBuffer({
        BufferSize,(float * ) Buffer.data()
    }));

    TArray <uint16> Indexes = {0,1,2};

    TriIB.reset(new FIndexBuffer({
        uint32(Indexes.size() * sizeof(uint16)),
        Indexes.data()
    }));


}
