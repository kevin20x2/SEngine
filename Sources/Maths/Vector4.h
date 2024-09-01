//
// Created by kevin on 2024/8/18.
//

#ifndef VECTOR4_H
#define VECTOR4_H
#include "Vector.h"
#include "Core/BaseTypes.h"


/*
struct FVector4
{
public:



    FVector4() {}
    FVector4(float InX ,float InY ,float InZ, float InW ):
    X(InX),Y(InY),Z(InZ),W(InW)
    {}

    FVector4(const FVector& V, float InW ) :
        X(V.X),Y(V.Y),Z(V.Z),W(InW) { }

    static float DotProduct(const FVector4 & A , const FVector4 & B);

    float operator[](int32 Index) const
    {
        if(Index >=4 ) abort();
        const float * Base = reinterpret_cast<const float *>(this);
        return Base[Index];
    }


    float X,Y,Z,W;
};
*/



#endif //VECTOR4_H
