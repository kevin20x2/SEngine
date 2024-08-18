//
// Created by kevin on 2024/8/17.
//

#ifndef VECTOR_H
#define VECTOR_H



struct FVector
{

    static FVector ZeroVector;
    static FVector OneVector;

    static float DotProduct(const FVector & A , const FVector & B );
    static FVector CrossProduct(const FVector & A ,const FVector & B);

    float X, Y, Z;
};



#endif //VECTOR_H
