//
// Created by vinkzheng on 2024/10/31.
//

#include "ShaderManager.h"

#include <filesystem>

#include "AssetManager/AssetManager.h"
#include "AssetManager/ShaderFileWatcher.h"
#include "Core/Log.h"
#include "Platform/Path.h"
#include "Rendering/Shader.h"

TSharedPtr<FShader> SShaderManager::CreateShader(const FString& ShaderFilePath)
{
    if(!ShaderFilePath.ends_with(".sshader")) return nullptr;

    auto VertexShader = SNew<FVertexShaderProgram>(ShaderFilePath);
    auto PixelShader = SNew<FPixelShaderProgram>(ShaderFilePath);
    auto Shader = TSharedPtr<FShader>(new FShader( VertexShader,PixelShader));

    Shader->SetShaderPath(ShaderFilePath);
    auto NameWithExt =  FPath::GetFileNameFromPath(ShaderFilePath);
    auto Name =NameWithExt.substr(0, NameWithExt.find_last_of('.'));
    Shader->SetName(Name);

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

FShader* SShaderManager::GetShaderFromName(const FString& Name)
{
    auto This = GetEngineModule<SShaderManager>();
    if(This && This->ShaderNameMap.find(Name) != This->ShaderNameMap.end())
    {
        return This->ShaderNameMap[Name].lock().get();
    }
    return nullptr;
}

void SShaderManager::AddShader(TSharedPtr<FShader> InShader)
{
    Shaders.Add(InShader);

    if(InShader)
    {
        ShaderFileMap[InShader->GetShaderPath()] = InShader;
        ShaderNameMap[InShader->GetName()] = InShader;
    }
}

void SShaderManager::OnInitialize()
{
    SEngineModuleBase::OnInitialize();
    InitShaders();
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

void SShaderManager::InitShaders()
{
    for(auto const & DirEntry : std::filesystem::recursive_directory_iterator{FPath::GetEngineShaderDir()})
    {
        auto PathString = DirEntry.path().string();
        SLogD(TEXT("Init Shader path : {} "),PathString);
        PathString = FPath::NormalizePath(PathString);
        CreateShader(PathString);
    }
}


