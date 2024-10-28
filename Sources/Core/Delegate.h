//
// Created by vinkzheng on 2024/10/24.
//

#ifndef DELEGATE_H
#define DELEGATE_H


struct FDelegateContainer
{

};


template <typename T>
class TDelegate
{
    static_assert(sizeof(T) == 0 , "T must not be zero");
};

template <typename InRetValue , typename... Args>
class TDelegate<InRetValue (Args...)> : public FDelegateContainer
{
    using FuncType = InRetValue (Args...);

};


#endif //DELEGATE_H
