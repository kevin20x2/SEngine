//
// Created by kevin on 2024/9/3.
//

#ifndef ENGINE_H
#define ENGINE_H
#include "Input/Input.h"
#include "Rendering/Renderer.h"


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

    void Tick(float DeltaTime);
protected:
    SWorld * World;

    FInput * Input;

    FRenderer * Renderer;

    TSharedPtr<SLocalPlayer> LocalPlayer;
};

extern FEngine * GEngine;


#endif //ENGINE_H
