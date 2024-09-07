//
// Created by kevin on 2024/9/8.
//

#ifndef INPUT_H
#define INPUT_H
#include "Core/BaseTypes.h"
#include "GLFW/glfw3.h"


void RawInputEvent(GLFWwindow * window,int k, int s, int action,int mods);

class FInput
{
public:
    using FKeyCallFuncType = TFunction<void ()>;

    void BindKey(int32 key , FKeyCallFuncType && Func );

    void BroadCastKeyPress(int32 Key);

protected:
    TMap <int32, TArray <FKeyCallFuncType> > BindingMaps;
};



#endif //INPUT_H
