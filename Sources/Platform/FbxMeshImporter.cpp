//
// Created by kevin on 2024/9/8.
//

#include "FbxMeshImporter.h"
#include "FbxCommon/Common.h"


bool FFbxMeshImporter::ImportMesh(const FString& FilePath, FStaticMesh* InStaticMesh)
{
    FbxManager * SdkManager = nullptr;
    FbxScene * Scene = nullptr;

    InitializeSdkObjects(SdkManager,Scene);


    bool LoadResult = false;

    LoadResult = LoadScene(SdkManager,Scene,FilePath.c_str());

    if(!LoadResult)
    {
        printf("fail to load file %s",FilePath.c_str());
    }

    return false;
}
