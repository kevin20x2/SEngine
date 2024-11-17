//
// Created by kevin on 2024/9/3.
//

#ifndef ENGINE_H
#define ENGINE_H
#include "Input/Input.h"
#include "Rendering/Renderer.h"


class SAssetManager;
class SEngineModuleBase;
class FWindow;
class FImGUIPort;
class SLocalPlayer;
class SWorld;


class FEngine
{
public:

    FEngine();

    void Initialize();

    void OnInitialize();
    void OnPostInit();

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

    template <typename ModuleType>
    ModuleType * GetModuleByClass() const
    {
        for( auto Module : Modules )
        {
            if(Module != nullptr &&
                NanoRtti::DynCast<ModuleType*>(Module.get()) != nullptr)
            {
                return (ModuleType *)Module.get();
            }
        }
        return nullptr;
    }


    void UpdateTime();

    void Tick();
protected:
    SWorld * World;

    FInput * Input;

    FRenderer * Renderer;

    TSharedPtr<SLocalPlayer> LocalPlayer;

    TSharedPtr<FImGUIPort> ImGUIPort;

    TSharedPtr<FWindow> Window;

   // TSharedPtr<SAssetManager> AssetManager;
    TArray <TSharedPtr <SEngineModuleBase > > Modules;

    float DeltaTime ;
    float CurrentTimeInSeconds;

};

extern FEngine * GEngine;


#endif //ENGINE_H
