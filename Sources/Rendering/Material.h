//
// Created by 郑文凯 on 2024/10/2.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "Shader.h"
#include "RHI/GraphicsPipeline.h"
#include "RHI/UniformBuffer.h"


class SMaterialInterface
{
public:
	explicit SMaterialInterface(TSharedPtr<FShader> InShader) :
		Shader(InShader)
	{
	}

	virtual void OnRecordCommandBuffer(VkCommandBuffer CommandBuffer,uint32 CurrentFrame);

	void Initialize(VkDescriptorPool Pool ,FRenderPass * RenderPass);

	virtual void SetupViewData(const TArray <TSharedPtr<FUniformBuffer> > & InUniformBuffers);
	virtual void CreateDescriptionSets(VkDescriptorPool Pool);
	virtual void CreatePipeline(FRenderPass * RenderPass);
	virtual void InitMaterialParameters();

	void SetTexture(uint32 Binding , TSharedPtr <FTexture> InTexture );
protected:


	TSharedPtr <FShader> Shader;
	TArray <VkDescriptorSet> DescriptionSets;
	FMaterialParameters MaterialParameters;
	TSharedPtr <FGraphicsPipeline> GraphicPipeline;
};


#endif //MATERIAL_H
