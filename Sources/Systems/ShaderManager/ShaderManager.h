//
// Created by vinkzheng on 2024/10/31.
//

#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H
#include "CoreObjects/EngineModuleBase.h"


class FPixelShaderProgram;
class FVertexShaderProgram;
class SShader;

class SShaderManager : public SEngineModuleBase
{
    S_REGISTER_CLASS(SEngineModuleBase)
public:
    static TSharedPtr <SShader> CreateShader(const FString & ShaderFilePath);

    static TSharedPtr<SShader> CreateShader(TSharedPtr<FVertexShaderProgram> InVertex,
        TSharedPtr <FPixelShaderProgram> InPixel );

    static SShader * GetShaderFromName(const FString & Name);

    void AddShader(TSharedPtr <SShader> InShader);

    virtual void OnInitialize() override;

    virtual void OnPostInit() override;

    void OnShaderFileChanged(const FString & FilePath);

    virtual bool IsTickable() const override { return true;}

    virtual void Tick(float DeltaTime) override;


protected:

    void InitShaders();

    TArray <TSharedPtr<SShader>>  Shaders;
    TMap <FString, TWeakPtr <SShader> > ShaderFileMap;
    TMap <FString , bool> ShaderDirtyStates;
    TMap <FString, TWeakPtr <SShader> > ShaderNameMap;
};



#endif //SHADERMANAGER_H
