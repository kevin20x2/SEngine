//
// Created by kevin on 2024/9/8.
//

#include "Input.h"

#include <cstdio>

#include "CoreObjects/Engine.h"
#include "spdlog/spdlog.h"


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

void RawScrollCallback(GLFWwindow* Window, double x, double y)
{
    //spdlog::info("Scroll :  {} {}\n",x,y);

    FInput * Input = nullptr;
    if(GEngine)
    {
        Input = GEngine->GetInput();
    }
    if(!Input) return ;
    Input->BroadCastScrollOperation(x,y);
}

void RawMouseButtonCallback(GLFWwindow* Window, int button, int action, int mods)
{
    FInput* Input = nullptr;
    if(GEngine)
    {
        Input = GEngine->GetInput();
    }
    if(!Input) return ;

    double X , Y;

    glfwGetCursorPos(Window,&X,&Y);

    if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if(action == GLFW_PRESS)
        {
            //spdlog::info("Left Pressed");
            Input->OnMousePressed(EMouseType::Left,X,Y);
        }
        else if(action == GLFW_RELEASE)
        {
            //spdlog::info("Left Released");
            Input->OnMouseReleased(EMouseType::Left,X,Y);
        }
    }
}

void RawCursorPositionCallback(GLFWwindow* Window, double x, double y)
{
    //spdlog::info("cursor moved {} {}",x,y);

    FInput* Input = nullptr;
    if(GEngine)
    {
        Input = GEngine->GetInput();
    }
    if(!Input) return ;

    Input->OnCursorPosition(x,y);
}

void FInput::BroadCastKeyPress(int32 Key)
{
    if(KeyBindingMaps.find(Key) != KeyBindingMaps.end())
    {
        for(auto & Func : KeyBindingMaps[Key])
        {
            Func();
        }
    }
}

void FInput::BindScrollOperation(FScrollCallFuncType&& Func)
{
    ScrollBindings.push_back(Func);
}

void FInput::BroadCastScrollOperation(double x, double y)
{
    for(auto & Func : ScrollBindings)
    {
        Func(y);
    }
}

void FInput::BroadCastMousePressed(EMouseType Type)
{
    if(MousePressBindings.find(Type) != MousePressBindings.end())
    {
        for(auto & Func : MousePressBindings[Type])
        {
            Func();
        }
    }
}

void FInput::BroadCastMouseReleased(EMouseType Type)
{
    if(MouseReleaseBindings.find(Type) != MouseReleaseBindings.end())
    {
        for(auto & Func : MouseReleaseBindings[Type])
        {
            Func();
        }
    }
}

void FInput::OnMousePressed(EMouseType Type, double X, double Y)
{
    MousePressStates[Type] = {true,false, X,Y } ;
    BroadCastMousePressed(Type);
}

void FInput::OnMouseReleased(EMouseType Type, double X, double Y)
{
    if(MousePressStates[Type].bDragBegin )
    {
        OnDragEnd(Type);
    }
    MousePressStates[Type] = {false,false,X,Y};
    BroadCastMouseReleased(Type);
}

void FInput::OnCursorPosition(double x, double y)
{
    constexpr double DragDisThreasHold = 1.5;
    for(auto & [Type , State] : MousePressStates)
    {
        if(State.bPressed)
        {
            //spdlog::info("Drag {:03.2f}, {:03.2f}",x,y);
            FVector2 Delta ;
            Delta.x =(float)(x -State.PressX );
            Delta.y =(float)(y -State.PressY );
            if(Delta.length() >= DragDisThreasHold
                && ! State.bDragBegin)
            {
                State.bDragBegin = true;
                OnDragBegin(Type);
            }
            else if(State.bDragBegin)
            {
                OnDragging(Type,Delta.x,Delta.y);
            }

        }
    }

}

void FInput::OnDragBegin(EMouseType Type)
{
    spdlog::info("Drag begin ");
    for(auto & Func : DragBeginBindings)
    {
        Func(Type);
    }
}

void FInput::OnDragging(EMouseType Type, float DeltaX, float DeltaY)
{
    spdlog::info("Draging {} {} ",DeltaX,DeltaY);
    for(auto & Func : DraggingBindings)
    {
        Func(Type,DeltaX,DeltaY);
    }
}

void FInput::OnDragEnd(EMouseType Type)
{
    spdlog::info("Drag end");
    for(auto & Func : DragEndBindings)
    {
        Func(Type);
    }
}

void FInput::RegisterDragBegin(FDragBeginFuncType&& Func)
{
}

void FInput::RegisterDragEnd(FDragEndFuncType&& Func)
{
}

void FInput::RegisterDragging(FDraggingFuncType&& Func)
{
}

void FInput::BindKey(int key, FKeyCallFuncType && Func)
{
    if(KeyBindingMaps.find(key) != KeyBindingMaps.end())
    {
        KeyBindingMaps[key].push_back(Func);
    }
    else
    {
        KeyBindingMaps[key] = {Func};
    }
}
