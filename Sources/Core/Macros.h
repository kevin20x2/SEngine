//
// Created by kevin on 2024/11/23.
//

#ifndef MACROS_H
#define MACROS_H

#define SPROPERTY(Type,Var) \
    public:\
    const Type & Get##Var() const\
    {\
        return Var;\
    }\
    void Set##Var(const Type & InValue) { Var = InValue; } \
    protected:\
    Type Var;\



#endif //MACROS_H
