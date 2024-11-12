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
    static TSharedPtr<FShader> CreateShader(TSharedPtr<FVertexShaderProgram> InVertex,
        TSharedPtr <FPixelShaderProgram> InPixel );

    void AddShader(TSharedPtr <FShader> InShader);

protected:

    TArray <TSharedPtr<FShader>>  Shaders;
    TMap <FString, TWeakPtr <FShaderProgram> > ShaderProgramsMap;
};



#endif //SHADERMANAGER_H
