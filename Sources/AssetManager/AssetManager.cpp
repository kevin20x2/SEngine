//
// Created by 郑文凯 on 2024/10/1.
//

#include "AssetManager.h"

#include "CoreObjects/SObject.h"
#include "Platform/Path.h"
#include "AssetManager/ShaderFileWatcher.h"

FAssetManager::FAssetManager()
{
    ShaderFileWatcher = SNew<FShaderFileWatcher>(FPath::GetEngineShaderDir());
}
