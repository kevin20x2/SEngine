//
// Created by 郑文凯 on 2024/9/28.
//

#include "Path.h"
#include <filesystem>

FString FPath::JoinPath(const FString &Dir, const FString &File)
{
	return Dir + "/" +  File;
}

FString FPath::GetApplicationDir()
{
	return ROOT_DIR;
}

FString FPath::GetDirectoryFromPath(const FString& Path)
{
	std::filesystem::path stdpath = Path;
	return stdpath.parent_path().string();
}

FString FPath::GetEngineShaderDir()
{
	return JoinPath(ROOT_DIR, "Shaders");
}

FString FPath::GetAssetsDir()
{
	return JoinPath(ROOT_DIR, "Assets");
}
