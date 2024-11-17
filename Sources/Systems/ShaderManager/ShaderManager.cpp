//
// Created by vinkzheng on 2024/10/31.
//

#include "ShaderManager.h"

#include "AssetManager/AssetManager.h"
#include "AssetManager/ShaderFileWatcher.h"
#include "Core/Log.h"

TSharedPtr<FShader> SShaderManager::CreateShader(const FString& ShaderFilePath)
{
    auto VertexShader = SNew<FVertexShaderProgram>(ShaderFilePath);
    auto PixelShader = SNew<FPixelShaderProgram>(ShaderFilePath);
    auto Shader = TSharedPtr<FShader>(new FShader( VertexShader,PixelShader));

    Shader->SetShaderPath(ShaderFilePath);
    auto This = GetEngineModule<SShaderManager>();
    if(This)
    {
        This->AddShader(Shader);
    }
    return Shader;

}

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
        ShaderFileMap[InShader->GetShaderPath()] = InShader;
    }
}

void SShaderManager::OnPostInit()
{
    SEngineModuleBase::OnPostInit();

    auto AssetManager = GEngine->GetModuleByClass<SAssetManager>();
    if(AssetManager)
    {
        if(AssetManager->ShaderFileWatcher)
        {
            AssetManager->ShaderFileWatcher->OnShaderFileChanged.AddSP(AsShared(),
                &SShaderManager::OnShaderFileChanged );
        }
    }
}

void SShaderManager::OnShaderFileChanged(const FString& FilePath)
{
    SLogD(TEXT("ShaderFileChanged .. {}"),FilePath);
    if(ShaderFileMap.find(FilePath ) !=ShaderFileMap.end())
    {
        ShaderDirtyStates[FilePath] = true;
    }
}

void SShaderManager::Tick(float DeltaTime)
{
    SEngineModuleBase::Tick(DeltaTime);
    for(auto &[Path , State  ] : ShaderDirtyStates)
    {
        if(auto Shader = ShaderFileMap[Path].lock(); Shader && State)
        {
            Shader->OnReCompile();
            State = false;
        }
    }
}


