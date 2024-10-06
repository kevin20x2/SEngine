//
// Created by kevin on 2024/9/8.
//

#ifndef FBXMESHIMPORTER_H
#define FBXMESHIMPORTER_H
#include "MeshImporter.h"
#include "Core/BaseTypes.h"
#include "CoreObjects/Actor.h"


class SMaterialInterface;
class FStaticMesh;

class FFbxMeshImporter : public FMeshImporter
{
public:
    bool ImportMesh(const FString & FilePath, FStaticMesh * InStaticMesh );

    TSharedPtr<SActor> LoadAsSingleActor(const FString & FilePath,SMaterialInterface * DefaultMaterial);
};



#endif //FBXMESHIMPORTER_H
