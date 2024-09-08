//
// Created by kevin on 2024/9/8.
//

#ifndef INPUT_H
#define INPUT_H
#include "Core/BaseTypes.h"
#include "GLFW/glfw3.h"


void RawInputEvent(GLFWwindow * window,int k, int s, int action,int mods);

void RawScrollCallback(GLFWwindow * Window, double x, double y);

void RawMouseButtonCallback(GLFWwindow * Window , int button,int action,int mods);

void RawCursorPositionCallback(GLFWwindow * Window ,double x, double y);

enum class EMouseType
{
    Left ,
    Right
};

class FInput
{
public:
    using FKeyCallFuncType = TFunction<void ()>;

    using FScrollCallFuncType = TFunction<void (double)>;

    using FMouseCallFuncType = TFunction<void ()>;

    void BindKey(int32 key , FKeyCallFuncType && Func );
    void BroadCastKeyPress(int32 Key);

    void BindScrollOperation(FScrollCallFuncType && Func);
    void BroadCastScrollOperation(double x , double y);

    void BroadCastMousePressed(EMouseType Type);
    void BroadCastMouseReleased(EMouseType Type);

    void OnMousePressed(EMouseType Type,double X, double Y);
    void OnMouseReleased(EMouseType Type,double X, double Y);
    void OnCursorPosition(double x, double y);

    using FDragBeginFuncType = TFunction<void (EMouseType)>;
    using FDragEndFuncType = TFunction<void (EMouseType)>;
    using FDraggingFuncType = TFunction<void (EMouseType,float,float)>;

    void OnDragBegin(EMouseType Type);
    void OnDragging(EMouseType Type,float DeltaX, float DeltaY);
    void OnDragEnd(EMouseType Type);

    void RegisterDragBegin(FDragBeginFuncType && Func);
    void RegisterDragEnd(FDragEndFuncType && Func);
    void RegisterDragging(FDraggingFuncType && Func);

protected:
    TArray <FDragBeginFuncType> DragBeginBindings ;
    TArray <FDraggingFuncType> DraggingBindings ;
    TArray <FDragEndFuncType> DragEndBindings ;

protected:
    TMap <int32, TArray <FKeyCallFuncType> > KeyBindingMaps;

    TArray <FScrollCallFuncType> ScrollBindings;

    TMap <EMouseType , TArray <FMouseCallFuncType> > MousePressBindings;
    TMap <EMouseType , TArray <FMouseCallFuncType> > MouseReleaseBindings;

    struct PressState
    {
        bool bPressed = false;
        bool bDragBegin = false;
        double PressX;
        double PressY;
    };

    TMap <EMouseType, PressState> MousePressStates;
};



#endif //INPUT_H
