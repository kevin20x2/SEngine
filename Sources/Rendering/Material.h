//
// Created by 郑文凯 on 2024/10/2.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "Shader.h"
#include "CoreObjects/SObject.h"
#include "RHI/GraphicsPipeline.h"
#include "RHI/TextureCube.h"


struct FPreRecordBufferContext;
class FPrimitiveRenderData;

class SMaterialInterface : public SObject
{
	S_REGISTER_CLASS(SObject)
public:
	explicit SMaterialInterface(TSharedPtr<SShader> InShader) :
		Shader(InShader),bNeedViewData(true)
	{
	}

	virtual void OnPreRecordCommandBuffer(FPrimitiveRenderData * InRenderData,uint32 CurrentFrame,
		FPreRecordBufferContext & Context
		);
	virtual void OnRecordCommandBuffer(VkCommandBuffer CommandBuffer,uint32 CurrentFrame);

	void Initialize(VkDescriptorPool Pool ,FRenderPass * RenderPass);

	virtual void OnSetupViewData();
	virtual void OnSetupLightData();
	virtual void OnSetupPrimitiveData(FPrimitiveRenderData * InRenderData,uint32 CurrentFrame);

	virtual void CreateDescriptionSets(VkDescriptorPool Pool);
	virtual void CreatePipeline(FRenderPass * RenderPass);
	virtual void InitMaterialParameters();

	void SetTexture(uint32 Binding , TSharedPtr <FTexture2D> InTexture );
	void SetTextureCube(uint32 Binding , TSharedPtr <FTextureCubeRHI> CubeTexture);

    void SetTextureCube(const FString & Name , TSharedPtr <FTextureCubeRHI> CubeTexture);

	void SetTexture(const FString & Name, TSharedPtr <FTexture2D>  InTexture);

	void SetSampler(const FString & Name, TSharedPtr <FTextureBase> InTexture);

	bool SetVector(const FString & Name, const FVector4 & InVector);

    bool SetInt(const FString & Name, int32 IntValue);

	bool SetScalar(const FString & Name,float InValue);

    void SyncToCommandBuffer(VkCommandBuffer CommandBuffer);

	void MarkDirty()
	{
		bDirty = true;
	}
	bool IsDirty() const
	{
		return bDirty;
	}
	const TArray <VkDescriptorSet> &GetDescriptorSets() const
	{
		return DescriptionSets;
	}

protected:
	TSharedPtr <SShader> Shader;
	TArray <VkDescriptorSet> DescriptionSets;
	FMaterialParameters MaterialParameters;
	TSharedPtr <FGraphicsPipeline> GraphicPipeline;
	bool bDirty = false;
	SPROPERTY(bool,bNeedViewData);
};


#endif //MATERIAL_H
