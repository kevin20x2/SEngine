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
        This->Shaders.Add(Result);
    }
    return Result;
}
