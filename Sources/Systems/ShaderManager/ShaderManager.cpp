//
// Created by vinkzheng on 2024/10/31.
//

#include "ShaderManager.h"

TSharedPtr<FShader> SShaderManager::CreateShader(TSharedPtr<FVertexShaderProgram> InVertex,
    TSharedPtr<FPixelShaderProgram> InPixel)
{
    auto Result = TSharedPtr <FShader> (new FShader(InVertex, InPixel));

    auto This = GetEngineModule<SShaderManager>();
    if(This)
    {
        This->AddShader(Result);
    }
    return Result;
}

void SShaderManager::AddShader(TSharedPtr<FShader> InShader)
{
    Shaders.Add(InShader);

    if(InShader)
    {
        if(auto VertexShader = InShader->GetVertexShader())
        {
            ShaderProgramsMap[VertexShader->GetShaderFilePath()] = VertexShader->weak_from_this() ;
        }

        if(auto PixelShader = InShader->GetPixelShader())
        {
            ShaderProgramsMap[PixelShader->GetShaderFilePath()] = PixelShader->weak_from_this() ;
        }
    }
}
