//
// Created by kevin on 2024/9/8.
//

#ifndef INPUT_H
#define INPUT_H
#include "Core/BaseTypes.h"
#include "Core/Delegate.h"
#include "CoreObjects/EngineModuleBase.h"
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

class SInput : public SEngineModuleBase
{
    S_REGISTER_CLASS(SEngineModuleBase)
public:
    using FKeyCallFuncType = TFunction<void ()>;

    using FScrollCallFuncType = TFunction<void (double)>;

    using FMouseCallFuncType = TFunction<void ()>;

    virtual bool IsTickable() const override;
    virtual void Tick(float DeltaTime) override;

    void BindKey(int32 key , FKeyCallFuncType && Func );
    void BroadCastKeyPress(int32 Key);
    void BroadCastKeyRelease(int32 Key);

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

    auto & GetOnKeyPressed() {return OnKeyPressed;}
    auto & GetOnKeyReleased() {return OnKeyReleased;}
    auto & GetOnKeyPressing() { return OnKeyPressing;}

protected:
    TArray <FDragBeginFuncType> DragBeginBindings ;
    TArray <FDraggingFuncType> DraggingBindings ;
    TArray <FDragEndFuncType> DragEndBindings ;

protected:

    MulticastDelegate<int32> OnKeyPressed;
    MulticastDelegate<int32> OnKeyReleased;
    MulticastDelegate<int32> OnKeyPressing;

    TMap <int32, bool> KeyPressingMap ;

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
