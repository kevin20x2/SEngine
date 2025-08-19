//
// Created by kevin on 2024/9/8.
//

#include "FbxMeshImporter.h"

#include <spdlog/spdlog.h>

#include "CoreObjects/PrimitiveComponent.h"
#include "FbxCommon/Common.h"
#include "Rendering/Mesh.h"

struct FTravelFbxNodeContext
{
    FStaticMesh * Mesh = nullptr;
	SSceneComponent * CurSceneComponent = nullptr;
	SPrimitiveComponent * PrimitiveComponent = nullptr;
	SActor * Actor = nullptr;
	SMaterialInterface * DefaultMaterial = nullptr;
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
    //printf("MeshName : %s\n",(char *)pMesh->GetName());
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
		Context.Mesh->Normals[PointIdx] += (FVector4(Normal[0],Normal[1],Normal[2],Normal[3]));
	};


	int32 VertexId = 0 ;
    for(int i = 0; i < pMesh->GetPolygonCount(); ++i)
    {
        int32 PolygonSize = pMesh->GetPolygonSize(i);

    	// index
    	if(PolygonSize >=3 )
    	{
    		for(int32 vIdx = 2; vIdx < PolygonSize; ++vIdx)
    		{
			    const int idx0 = pMesh->GetPolygonVertex(i, 0);
			    const int idx1 = pMesh->GetPolygonVertex(i, vIdx - 1);
			    int idx2 = pMesh->GetPolygonVertex(i, vIdx);

    			if(idx0 < 0 || idx1 < 0 || idx2 < 0) continue;

				Context.Mesh->Indexes.push_back(idx0 + VertexOffset);
				Context.Mesh->Indexes.push_back(idx2 + VertexOffset);
				Context.Mesh->Indexes.push_back(idx1 + VertexOffset);
    		}
    	}


        for(int32 j = 0 ; j < PolygonSize; ++ j)
        {
            int32 PointIndex = pMesh->GetPolygonVertex(i,j);
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

	for(auto & Normal : Context.Mesh->Normals)
	{
		Normal /= Normal.w;
	}

}

static void TravelFbxNodeResursive(FbxNode * InNode,FTravelFbxNodeContext & Context)
{
    {
    	//auto Translation = InNode->GetGeometricTranslation(FbxNode::eDestinationPivot);
    	//auto Rotation = InNode->GetGeometricRotation(FbxNode::eDestinationPivot );
    	//auto Scale = InNode->GetGeometricScaling(FbxNode::eDestinationPivot);

    	FTransform Transform;
    	//Transform.Location = FVector( Translation.mData[0],Translation.mData[1],Translation.mData[2]);
    	//Transform.Scale = FVector(Scale.mData[0],Scale.mData[1],Scale.mData[2]);

    	FbxAMatrix Global ,Local;

    	Global = InNode->EvaluateGlobalTransform();
    	Local = InNode->EvaluateLocalTransform();

    	auto FbxTrans = Global.GetT();
    	auto FbxScale = Global.GetS();
    	Transform.Location = FVector(FbxTrans[0],FbxTrans[1],FbxTrans[2]);
    	Transform.Scale = FVector(FbxScale[0],FbxScale[1],FbxScale[2]);


	    spdlog::info("NodeName {} , Transform : {} {} ", InNode->GetName(), ToString(Transform.Location),ToString(Transform.Scale));
        //if(AttributeType == FbxNodeAttribute::eMesh)
	    if (InNode->GetNodeAttribute() && InNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
        	if(Context.Actor)
        	{
			    TSharedPtr<SPrimitiveComponent> NewPrimitive =
			    	TSharedPtr<SPrimitiveComponent>(new SPrimitiveComponent(Context.Actor->AsShared()));
        		NewPrimitive->SetName(InNode->GetName());
			    Context.Actor->AddComponent(NewPrimitive);
    			if(!Context.Actor->GetRootComponent())
    			{
    				Context.Actor->SetRootComponent(NewPrimitive.get());
    			}
        		auto NewStaticMesh = SNew<FStaticMesh>();
        		Context.Mesh = NewStaticMesh.get();
        		CollectMesh(InNode, Context);
        		NewPrimitive->SetStaticMesh(NewStaticMesh);
        		NewPrimitive->SetMaterial(Context.DefaultMaterial->AsShared());
			    NewPrimitive->OnRegister();
			    if (Context.CurSceneComponent)
    			{
    				NewPrimitive->AttachToParent(Context.CurSceneComponent,ESceneComponentAttachmentRule::KeepWorldTransform);
    			}
			    Context.CurSceneComponent = NewPrimitive.get();
    			NewPrimitive->SetWorldTransform(Transform);
        	}
	    	else
	    	{
	    		CollectMesh(InNode,Context);
	    	}
        }
    	else
    	{
    		if(Context.Actor)
    		{
			    TSharedPtr<SSceneComponent> NewSceneComp = TSharedPtr<SSceneComponent>(new SSceneComponent(Context.Actor->AsShared()));
			    NewSceneComp->SetName(InNode->GetName());
			    Context.Actor->AddComponent(NewSceneComp);
    			if(!Context.Actor->GetRootComponent())
    			{
    				Context.Actor->SetRootComponent(NewSceneComp.get());
    			}
			    NewSceneComp->OnRegister();
    			if(Context.CurSceneComponent)
    			{
    				NewSceneComp->AttachToParent(Context.CurSceneComponent,ESceneComponentAttachmentRule::KeepWorldTransform);
    			}
			    Context.CurSceneComponent = NewSceneComp.get();
    			NewSceneComp->SetWorldTransform(Transform);
    		}
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

TSharedPtr<SActor> FFbxMeshImporter::LoadAsSingleActor(const FString& FilePath, SMaterialInterface* DefaultMaterial)
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
    	TSharedPtr <SActor> NewActor = TSharedPtr<SActor> (new SActor );
    	NewActor->SetName("NewActor");
        FbxNode* RootNode = Scene->GetRootNode();


    	FTravelFbxNodeContext Context =
    		{nullptr,nullptr,nullptr,NewActor.get(),DefaultMaterial} ;
    	TravelFbxNodeResursive(RootNode, Context);

	    DestroySdkObjects(SdkManager, LoadResult);
    	return NewActor;
    }
    DestroySdkObjects(SdkManager,LoadResult);
	return nullptr;
}
