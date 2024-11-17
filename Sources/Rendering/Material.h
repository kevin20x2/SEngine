//
// Created by 郑文凯 on 2024/10/2.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "Shader.h"
#include "CoreObjects/SObject.h"
#include "RHI/GraphicsPipeline.h"


struct FPreRecordBufferContext;
class FPrimitiveRenderData;

class SMaterialInterface : public SObject
{
	S_REGISTER_CLASS(SObject)
public:
	explicit SMaterialInterface(TSharedPtr<FShader> InShader) :
		Shader(InShader)
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

	void SetTexture(uint32 Binding , TSharedPtr <FTexture> InTexture );

	void MarkDirty()
	{
		bDirty = true;
	}
	bool IsDirty() const
	{
		return bDirty;
	}

protected:

	TSharedPtr <FShader> Shader;
	TArray <VkDescriptorSet> DescriptionSets;
	FMaterialParameters MaterialParameters;
	TSharedPtr <FGraphicsPipeline> GraphicPipeline;
	bool bDirty = false;
};


#endif //MATERIAL_H
