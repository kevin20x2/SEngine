//
// Created by kevin on 2024/9/3.
//

#ifndef ENGINE_H
#define ENGINE_H
#include "SObject.h"


class SInput;
class SAssetManager;
class SEngineModuleBase;
class FWindow;
class SImGUI;
class SLocalPlayer;
class SWorld;
class SRenderer;



class FEngine
{
public:

    FEngine();

    void Initialize();

    void OnInitialize();
    void OnPostInit();

    SInput * GetInput() const
    {
        return Input.get();
    }

    SRenderer * GetRenderer() const
    {
        return Renderer.get();
    }

    SLocalPlayer * GetLocalPlayer() const
    {
        return LocalPlayer.get();
    }

    void SetWindow(TSharedPtr<FWindow> InWindow)
    {
        Window = InWindow;
    }

    SImGUI * GetGUIPort() const
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

    TSharedPtr <SInput> Input;

    TSharedPtr<SRenderer>  Renderer;

    TSharedPtr<SLocalPlayer> LocalPlayer;

    TSharedPtr<SImGUI> ImGUIPort;

    TSharedPtr<FWindow> Window;

   // TSharedPtr<SAssetManager> AssetManager;
    TArray <TSharedPtr <SEngineModuleBase > > Modules;

    float DeltaTime ;
    float CurrentTimeInSeconds;

};

extern FEngine * GEngine;


#endif //ENGINE_H
