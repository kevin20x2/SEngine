//
// Created by kevin on 2024/9/8.
//

#include "Input.h"

#include <cstdio>

#include "CoreObjects/Engine.h"
#include "Maths/Math.h"
#include "spdlog/spdlog.h"


void RawInputEvent(GLFWwindow* window, int k, int s, int action, int mods)
{
    if(action != GLFW_PRESS && action != GLFW_RELEASE) return;

    SInput * Input = nullptr;
    if(GEngine)
    {
        Input = GEngine->GetInput();
    }
    if(!Input) return ;

#define INPUT_BROADCAST_CASE(KEY) \
    case GLFW_KEY_##KEY : \
        if(action == GLFW_PRESS) Input->BroadCastKeyPress(GLFW_KEY_##KEY);\
        if(action == GLFW_RELEASE) Input->BroadCastKeyPress(GLFW_KEY_##KEY);\
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

    SInput * Input = nullptr;
    if(GEngine)
    {
        Input = GEngine->GetInput();
    }
    if(!Input) return ;
    Input->BroadCastScrollOperation(x,y);
}

void RawMouseButtonCallback(GLFWwindow* Window, int button, int action, int mods)
{
    SInput* Input = nullptr;
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

    SInput* Input = nullptr;
    if(GEngine)
    {
        Input = GEngine->GetInput();
    }
    if(!Input) return ;

    Input->OnCursorPosition(x,y);
}

void SInput::BroadCastKeyPress(int32 Key)
{
    OnKeyPressed.Broadcast(Key);
    if(KeyBindingMaps.find(Key) != KeyBindingMaps.end())
    {
        for(auto & Func : KeyBindingMaps[Key])
        {
            Func();
        }
    }
}

void SInput::BroadCastKeyRelease(int32 Key)
{
    OnKeyReleased.Broadcast(Key);
}

void SInput::BindScrollOperation(FScrollCallFuncType&& Func)
{
    ScrollBindings.push_back(Func);
}

void SInput::BroadCastScrollOperation(double x, double y)
{
    for(auto & Func : ScrollBindings)
    {
        Func(y);
    }
}

void SInput::BroadCastMousePressed(EMouseType Type)
{
    if(MousePressBindings.find(Type) != MousePressBindings.end())
    {
        for(auto & Func : MousePressBindings[Type])
        {
            Func();
        }
    }
}

void SInput::BroadCastMouseReleased(EMouseType Type)
{
    if(MouseReleaseBindings.find(Type) != MouseReleaseBindings.end())
    {
        for(auto & Func : MouseReleaseBindings[Type])
        {
            Func();
        }
    }
}

void SInput::OnMousePressed(EMouseType Type, double X, double Y)
{
    MousePressStates[Type] = {true,false, X,Y } ;
    BroadCastMousePressed(Type);
}

void SInput::OnMouseReleased(EMouseType Type, double X, double Y)
{
    if(MousePressStates[Type].bDragBegin )
    {
        OnDragEnd(Type);
    }
    MousePressStates[Type] = {false,false,X,Y};
    BroadCastMouseReleased(Type);
}

void SInput::OnCursorPosition(double x, double y)
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

void SInput::OnDragBegin(EMouseType Type)
{
    spdlog::info("Drag begin ");
    for(auto & Func : DragBeginBindings)
    {
        Func(Type);
    }
}

void SInput::OnDragging(EMouseType Type, float DeltaX, float DeltaY)
{
    spdlog::info("Draging {} {} ",DeltaX,DeltaY);
    for(auto & Func : DraggingBindings)
    {
        Func(Type,DeltaX,DeltaY);
    }
}

void SInput::OnDragEnd(EMouseType Type)
{
    spdlog::info("Drag end");
    for(auto & Func : DragEndBindings)
    {
        Func(Type);
    }
}

void SInput::RegisterDragBegin(FDragBeginFuncType&& Func)
{
	DragBeginBindings.emplace_back(Func);
}

void SInput::RegisterDragEnd(FDragEndFuncType&& Func)
{
	DragEndBindings.emplace_back(Func);
}

void SInput::RegisterDragging(FDraggingFuncType&& Func)
{
	DraggingBindings.emplace_back(Func);
}

void SInput::BindKey(int key, FKeyCallFuncType && Func)
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
