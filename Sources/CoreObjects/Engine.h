//
// Created by kevin on 2024/9/3.
//

#ifndef ENGINE_H
#define ENGINE_H
#include "Input/Input.h"
#include "Rendering/Renderer.h"


class FWindow;
class FImGUIPort;
class SLocalPlayer;
class SWorld;


class FEngine
{
public:

    FEngine();

    void Initialize();

    FInput * GetInput() const
    {
        return Input;
    }

    FRenderer * GetRenderer() const
    {
        return Renderer;
    }

    SLocalPlayer * GetLocalPlayer() const
    {
        return LocalPlayer.get();
    }

    void SetWindow(TSharedPtr<FWindow> InWindow)
    {
        Window = InWindow;
    }

    FImGUIPort * GetGUIPort() const
    {
        return ImGUIPort.get() ;
    }

    void Tick(float DeltaTime);
protected:
    SWorld * World;

    FInput * Input;

    FRenderer * Renderer;

    TSharedPtr<SLocalPlayer> LocalPlayer;

    TSharedPtr <FImGUIPort> ImGUIPort;

    TSharedPtr<FWindow> Window;
};

extern FEngine * GEngine;


#endif //ENGINE_H
