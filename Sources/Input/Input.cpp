//
// Created by kevin on 2024/9/8.
//

#include "Input.h"

#include <cstdio>

#include "CoreObjects/Engine.h"


void RawInputEvent(GLFWwindow* window, int k, int s, int action, int mods)
{
    if(action != GLFW_PRESS) return;

    FInput * Input = nullptr;
    if(GEngine)
    {
        Input = GEngine->GetInput();
    }
    if(!Input) return ;

#define INPUT_BROADCAST_CASE(KEY) \
    case GLFW_KEY_##KEY : \
        Input->BroadCastKeyPress(GLFW_KEY_##KEY);\
        break;

    switch (k)
    {
        INPUT_BROADCAST_CASE(A)
        INPUT_BROADCAST_CASE(B)
        INPUT_BROADCAST_CASE(C)
        INPUT_BROADCAST_CASE(D)
        INPUT_BROADCAST_CASE(E)
        INPUT_BROADCAST_CASE(F)
        INPUT_BROADCAST_CASE(G)
        INPUT_BROADCAST_CASE(H)
        INPUT_BROADCAST_CASE(I)
        INPUT_BROADCAST_CASE(J)
        INPUT_BROADCAST_CASE(K)
        INPUT_BROADCAST_CASE(L)
        INPUT_BROADCAST_CASE(M)
        INPUT_BROADCAST_CASE(N)
        INPUT_BROADCAST_CASE(O)
        INPUT_BROADCAST_CASE(P)
        INPUT_BROADCAST_CASE(Q)
        INPUT_BROADCAST_CASE(R)
        INPUT_BROADCAST_CASE(S)
        INPUT_BROADCAST_CASE(T)
        INPUT_BROADCAST_CASE(U)
        INPUT_BROADCAST_CASE(V)
        INPUT_BROADCAST_CASE(W)
        INPUT_BROADCAST_CASE(X)
        INPUT_BROADCAST_CASE(Y)
        INPUT_BROADCAST_CASE(Z)
    }
}

void FInput::BroadCastKeyPress(int32 Key)
{
    if(BindingMaps.find(Key) != BindingMaps.end())
    {
        for(auto & Func : BindingMaps[Key])
        {
            Func();
        }
    }
}

void FInput::BindKey(int key, FKeyCallFuncType && Func)
{
    if(BindingMaps.find(key) != BindingMaps.end())
    {
        BindingMaps[key].push_back(Func);
    }
    else
    {
        BindingMaps[key] = {Func};
    }
}
