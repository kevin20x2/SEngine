//
// Created by 郑文凯 on 2024/10/1.
//

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include "Core/BaseTypes.h"
#include "CoreObjects/EngineModuleBase.h"

class FShaderFileWatcher;

enum class EAssetType
{
	None = 0 ,
	FbxModel,
	Shader ,
	Texture,
};

class SAssetManager : public SEngineModuleBase
{
	public:
	S_REGISTER_CLASS(SEngineModuleBase);

	SAssetManager() ;

	TSharedPtr<FShaderFileWatcher> ShaderFileWatcher;
};


#endif //ASSETMANAGER_H
