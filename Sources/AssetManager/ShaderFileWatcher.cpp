//
// Created by vinkzheng on 2024/10/28.
//

#include "ShaderFileWatcher.h"

#include "Core/Log.h"

void FShaderFileWatcher::OnFileModified(const FString &InPath)
{
    SLogD("FShaderFileWatcher::OnFileModified {}",InPath);
}
