//
// Created by kevin on 2024/8/18.
//

#ifndef MATRIX4_H
#define MATRIX4_H
#include "Vector4.h"
#include "Core/BaseTypes.h"


/*
struct FMatrix4
{
    FVector4 Col(int32 Index) const
    {
        return M4[Index];
    }
    FVector4 Row(int32 Index) const
    {
        return FVector4(M4[0][Index],M4[1][Index],M4[2][Index],M4[3][Index]);
    }

    FMatrix4 Mul(const FMatrix4 & Rhs) const;

    FMatrix4();

    union
    {
        FVector4 M4[4];
        float MM[4][4];
        float M[16];
    };
};
*/



#endif //MATRIX4_H
