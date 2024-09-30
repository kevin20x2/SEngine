//
// Created by kevin on 2024/9/8.
//

#include "FbxMeshImporter.h"
#include "FbxCommon/Common.h"
#include "Rendering/Mesh.h"

struct FTravelFbxNodeContext
{
    FStaticMesh * Mesh;
};

static void CollectVertexColor(FbxMesh* pMesh, FTravelFbxNodeContext& Context,int32 PointIndex,int32 VertexId)
{
	auto AddColor = [&](const FbxColor& Color)
	{
		Context.Mesh->Colors.push_back(FVector4(Color[0], Color[1], Color[2], Color[3]));
	};
	bool bHasVertexColor = false;
	for (int32 vertexColorIdx = 0; vertexColorIdx < pMesh->GetElementVertexColorCount(); ++vertexColorIdx)
	{
		FbxGeometryElementVertexColor* leVtxc = pMesh->GetElementVertexColor(vertexColorIdx);
		//FBXSDK_sprintf(header, 100, "            Color vertex: ");

		switch (leVtxc->GetMappingMode())
		{
		default:
			break;
		case FbxGeometryElement::eByControlPoint:
			switch (leVtxc->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				AddColor(leVtxc->GetDirectArray().GetAt(PointIndex));
				bHasVertexColor = true;
				break;
			case FbxGeometryElement::eIndexToDirect:
				{
					int id = leVtxc->GetIndexArray().GetAt(PointIndex);
					AddColor(leVtxc->GetDirectArray().GetAt(id));
					bHasVertexColor = true;
				}
				break;
			default:
				break; // other reference modes not shown here!
			}
			break;

		case FbxGeometryElement::eByPolygonVertex:
			{
				switch (leVtxc->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					AddColor(leVtxc->GetDirectArray().GetAt(VertexId));
					bHasVertexColor = true;
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						int id = leVtxc->GetIndexArray().GetAt(VertexId);
						AddColor(leVtxc->GetDirectArray().GetAt(id));
						bHasVertexColor = true;
					}
					break;
				default:
					break; // other reference modes not shown here!
				}
			}
			break;

		case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
		case FbxGeometryElement::eAllSame: // doesn't make much sense for UVs
		case FbxGeometryElement::eNone: // doesn't make much sense for UVs
			break;
		}
	} // Color
	if (bHasVertexColor)
	{
		Context.Mesh->ElementMask |= EMeshVertexElementMask::Color;
	}
}

static void CollectMesh(FbxNode * InNode, FTravelFbxNodeContext & Context)
{
    FbxMesh * pMesh = (FbxMesh*) InNode->GetNodeAttribute();
    printf("MeshName : %s\n",(char *)pMesh->GetName());
    FbxVector4 * lControlPoints = pMesh->GetControlPoints();
    int32 VertexOffset = Context.Mesh->Positions.size();

    // ControlPoints
    int32 ControlPointCount = pMesh->GetControlPointsCount();
    for(int32 i = 0 ; i < ControlPointCount; ++ i)
    {
        Context.Mesh->Positions.push_back( FVector( lControlPoints[i][0],lControlPoints[i][1],lControlPoints[i][2] ));
    }

    // Polygons
	Context.Mesh->Normals.resize(Context.Mesh->Positions.size());
	Context.Mesh->TexCoord0.resize(Context.Mesh->Positions.size());
	auto AddNormal =[&](int32 PointIdx,const FbxVector4 & Normal)
	{
		Context.Mesh->Normals[PointIdx] = (FVector4(Normal[0],Normal[1],Normal[2],Normal[3]));
	};


	int32 VertexId = 0 ;
    for(int i = 0; i < pMesh->GetPolygonCount(); ++i)
    {
        int32 PolygonSize = pMesh->GetPolygonSize(i);

        for(int32 j = 0 ; j < PolygonSize; ++ j)
        {
            int32 PointIndex = pMesh->GetPolygonVertex(i,j);
            Context.Mesh->Indexes.push_back(PointIndex + VertexOffset);
        	CollectVertexColor(pMesh,Context,PointIndex,VertexId);
        	bool bHasNormal = false;
			bool bHasUv = false;
	        auto AddUv = [&](int32 PointIdx, const FbxVector2 & Uv)
	        {
				bHasUv = true;
		        Context.Mesh->TexCoord0[PointIdx] = FVector2(Uv[0],Uv[1]);
	        };

	        for (int32 l = 0; l < pMesh->GetElementUVCount(); ++l)
	        {
		        FbxGeometryElementUV* leUV = pMesh->GetElementUV( l);
		        //FBXSDK_sprintf(header, 100, "            Texture UV: ");

		        switch (leUV->GetMappingMode())
		        {
		        default:
			        break;
		        case FbxGeometryElement::eByControlPoint:
			        switch (leUV->GetReferenceMode())
			        {
			        case FbxGeometryElement::eDirect:
				        AddUv(PointIndex, leUV->GetDirectArray().GetAt(VertexId));
				        break;
			        case FbxGeometryElement::eIndexToDirect:
			        {
				        int id = leUV->GetIndexArray().GetAt(VertexId);
				        AddUv(PointIndex, leUV->GetDirectArray().GetAt(id));
			        }
				        break;
			        default:
				        break; // other reference modes not shown here!
			        }
			        break;

		        case FbxGeometryElement::eByPolygonVertex:
		        {
			        int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
			        switch (leUV->GetReferenceMode())
			        {
			        case FbxGeometryElement::eDirect:
			        case FbxGeometryElement::eIndexToDirect:
			        {
				        AddUv(PointIndex, leUV->GetDirectArray().GetAt(lTextureUVIndex));
			        }
				        break;
			        default:
				        break; // other reference modes not shown here!
			        }
		        }
			        break;

		        case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
		        case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
		        case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
			        break;
		        }
	        }
			if(bHasUv)
			{
				Context.Mesh->ElementMask |= EMeshVertexElementMask::TexCoord0;
			}

            for (int32 l = 0; l < pMesh->GetElementNormalCount(); ++l)
            {
	            FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(l);

	            if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	            {
		            switch (leNormal->GetReferenceMode())
		            {
		            case FbxGeometryElement::eDirect:
			            AddNormal( PointIndex ,leNormal->GetDirectArray().GetAt(VertexId));
		            	bHasNormal = true;
			            break;
		            case FbxGeometryElement::eIndexToDirect:
			            {
				            int id = leNormal->GetIndexArray().GetAt(VertexId);
				            AddNormal(PointIndex, leNormal->GetDirectArray().GetAt(id));
		            	bHasNormal = true;
			            }
			            break;
		            default:
			            break; // other reference modes not shown here!
		            }
	            }
            }
        	if(bHasNormal)
        	{
        		Context.Mesh->ElementMask |= EMeshVertexElementMask::Normal;
        	}

        	VertexId++;
        } // polygon size
    } // polygon count

}

static void TravelFbxNodeResursive(FbxNode * InNode,FTravelFbxNodeContext & Context)
{
    if(InNode->GetNodeAttribute())
    {
    FbxNodeAttribute::EType AttributeType
        = InNode->GetNodeAttribute()->GetAttributeType();

        if(AttributeType == FbxNodeAttribute::eMesh)
        {
            CollectMesh(InNode,Context);
        }
    }

    for(int32 i = 0 ; i < InNode->GetChildCount() ; ++ i)
    {
        TravelFbxNodeResursive(InNode->GetChild(i),Context);
    }
}


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

    if(Scene)
    {
        FTravelFbxNodeContext Context {InStaticMesh};
        FbxNode* RootNode = Scene->GetRootNode();
        for(int32 i = 0 ; i < RootNode->GetChildCount() ; ++ i)
        {
            TravelFbxNodeResursive(RootNode->GetChild(i),Context);
        }
    }

    DestroySdkObjects(SdkManager,LoadResult);
    return false;
}
