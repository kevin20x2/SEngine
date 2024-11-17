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

    void AddShader(TSharedPtr <FShader> InShader);

    virtual void OnPostInit() override;

    void OnShaderFileChanged(const FString & FilePath);

protected:

    TArray <TSharedPtr<FShader>>  Shaders;
    TMap <FString, TWeakPtr <FShader> > ShaderFileMap;
};



#endif //SHADERMANAGER_H
