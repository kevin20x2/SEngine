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

FString FPath::GetFileNameFromPath(const FString& Path)
{
	std::filesystem::path stdPath = Path;
	return stdPath.filename().string();
}

static void StringReplaceAll(FString & Str, const FString & From ,const FString & To)
{
	if(From.empty()) return ;

	size_t StartPos = 0;

	while((StartPos = Str.find(From ,StartPos )) != std::string::npos)
	{
		Str.replace(StartPos,From.length(),To);
		StartPos += To.length();
	}
}

FString FPath::NormalizePath(const FString& Path)
{
	auto Result = Path;
	StringReplaceAll(Result, "\\","/" );
	return Result;
}

FString FPath::GetEngineShaderDir()
{
	return JoinPath(ROOT_DIR, "Shaders");
}

FString FPath::GetAssetsDir()
{
	return JoinPath(ROOT_DIR, "Assets");
}
