//
// Created by vinkzheng on 2024/10/31.
//

#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H
#include "CoreObjects/EngineModuleBase.h"


class FPixelShaderProgram;
class FVertexShaderProgram;
class FShader;

class SShaderManager : public SEngineModuleBase
{
    S_REGISTER_CLASS(SEngineModuleBase)
public:
    static TSharedPtr <FShader> CreateShader(const FString & ShaderFilePath);

    static TSharedPtr<FShader> CreateShader(TSharedPtr<FVertexShaderProgram> InVertex,
        TSharedPtr <FPixelShaderProgram> InPixel );

    static FShader * GetShaderFromName(const FString & Name);

    void AddShader(TSharedPtr <FShader> InShader);

    virtual void OnInitialize() override;

    virtual void OnPostInit() override;

    void OnShaderFileChanged(const FString & FilePath);

    virtual bool IsTickable() const override { return true;}

    virtual void Tick(float DeltaTime) override;


protected:

    void InitShaders();

    TArray <TSharedPtr<FShader>>  Shaders;
    TMap <FString, TWeakPtr <FShader> > ShaderFileMap;
    TMap <FString , bool> ShaderDirtyStates;
    TMap <FString, TWeakPtr <FShader> > ShaderNameMap;
};



#endif //SHADERMANAGER_H
