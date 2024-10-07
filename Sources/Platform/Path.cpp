//
// Created by 郑文凯 on 2024/9/28.
//

#include "Path.h"
#include <filesystem>
FString FPath::GetApplicationDir()
{
	return ROOT_DIR;
}

FString FPath::GetDirectoryFromPath(const FString& Path)
{
	std::filesystem::path stdpath = Path;
	return stdpath.parent_path().string();
}
