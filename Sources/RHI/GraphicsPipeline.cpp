//
// Created by kevin on 2024/8/11.
//

#include "GraphicsPipeline.h"

#include "RHI.h"
#include "volk.h"


FGraphicsPipeline::~FGraphicsPipeline()
{
	auto Device = *GRHI->GetDevice();
	vkDestroyPipeline(Device,Pipeline,nullptr);
	vkDestroyPipelineLayout(Device,Layout,nullptr);
}

FGraphicsPipeline::FGraphicsPipeline(FGrpahicsPipelineCreateInfo Info)
{
	VkPipelineShaderStageCreateInfo VertShaderStageInfo{};
	VertShaderStageInfo.sType =
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	VertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	VertShaderStageInfo.module = Info.Shader->GetVertexShader()->GetShaderModule();
	VertShaderStageInfo.pName = Info.Shader->GetVertexShader()->GetEntryPointName();

	VkPipelineShaderStageCreateInfo FragShaderStageInfo{};
	FragShaderStageInfo.sType =
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	FragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	FragShaderStageInfo.module = Info.Shader->GetPixelShader()->GetShaderModule();
	FragShaderStageInfo.pName = Info.Shader->GetPixelShader()->GetEntryPointName();

	VkPipelineShaderStageCreateInfo ShaderStages[] = { VertShaderStageInfo , FragShaderStageInfo };

	//VkPipelineVertexInputStateCreateInfo VertexInputInfo{};

	auto VertexInputInfo = Info.Shader->GetVertexInputInfo();

	auto VertexInputStateCreateInfo = VertexInputInfo.CreateInfo;
	VertexInputStateCreateInfo.vertexBindingDescriptionCount = VertexInputInfo.BindingDescription.size();
	VertexInputStateCreateInfo.pVertexBindingDescriptions = VertexInputInfo.BindingDescription.data();
	VertexInputStateCreateInfo.vertexAttributeDescriptionCount = VertexInputInfo.AttributeDescription.size();
	VertexInputStateCreateInfo.pVertexAttributeDescriptions = VertexInputInfo.AttributeDescription.data();
	//=  FStaticMesh::GenerateVertexInputStateCreateInfo(
		//EMeshVertexElementMask::Position | EMeshVertexElementMask::Normal | EMeshVertexElementMask::TexCoord0 );

	VkPipelineInputAssemblyStateCreateInfo InputAssembly{};
	InputAssembly.sType =
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	InputAssembly.primitiveRestartEnable = VK_FALSE;


	VkPipelineRasterizationStateCreateInfo RasterizationState{};
	RasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	RasterizationState.depthClampEnable = VK_FALSE;
	RasterizationState.rasterizerDiscardEnable = VK_FALSE;
	RasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
	RasterizationState.lineWidth = 1.0f;

	RasterizationState.cullMode = VK_CULL_MODE_NONE;
	RasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;

	RasterizationState.depthBiasEnable = VK_FALSE;
	RasterizationState.depthBiasConstantFactor = 0.0f;
	RasterizationState.depthBiasClamp = 0.0f;
	RasterizationState.depthBiasSlopeFactor = 0.0f;


	VkPipelineMultisampleStateCreateInfo Multisampling{};
	  Multisampling.sType =
		  VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	  Multisampling.sampleShadingEnable = VK_FALSE;
	  Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	  Multisampling.minSampleShading = 1.0f;
	  Multisampling.pSampleMask = nullptr;
	  Multisampling.alphaToCoverageEnable = VK_FALSE;
	  Multisampling.alphaToOneEnable = VK_FALSE;

	  VkPipelineDepthStencilStateCreateInfo DepthStencil {
		  .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		  .depthTestEnable = Info.ZTest,
		  .depthWriteEnable = Info.ZWrite,
		  .depthCompareOp = VK_COMPARE_OP_LESS,
		  .depthBoundsTestEnable = VK_FALSE,
		  .stencilTestEnable = VK_FALSE,
		  .front = {},
		  .back = {},
		  .minDepthBounds = 0.0f,
		  .maxDepthBounds = 1.0f,
	  };

	TArray < VkPipelineColorBlendAttachmentState> ColorBlendAttachments;
	ColorBlendAttachments.resize(Info.RenderPass->ColorAttachmentNum);

	for(uint32 Idx = 0 ; Idx < Info.RenderPass->ColorAttachmentNum; ++ Idx)
	{
		ColorBlendAttachments[Idx]= {
			.blendEnable = VK_FALSE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT,
		};
	}

  VkPipelineColorBlendStateCreateInfo ColorBlending{};
  ColorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  ColorBlending.logicOpEnable = VK_FALSE;
  ColorBlending.logicOp = VK_LOGIC_OP_COPY;
  ColorBlending.attachmentCount = Info.RenderPass->ColorAttachmentNum;
  ColorBlending.pAttachments = ColorBlendAttachments.data();
  ColorBlending.blendConstants[0] = 0.0f;
  ColorBlending.blendConstants[1] = 0.0f;
  ColorBlending.blendConstants[2] = 0.0f;
  ColorBlending.blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
  PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  PipelineLayoutInfo.setLayoutCount = 1;
  PipelineLayoutInfo.pSetLayouts = &Info.Shader->GetDescriptorSetLayoutRef();
  PipelineLayoutInfo.pushConstantRangeCount = 0;
  PipelineLayoutInfo.pPushConstantRanges = nullptr;

  VK_CHECK(vkCreatePipelineLayout(*GRHI->GetDevice(), &PipelineLayoutInfo, nullptr,
                                  &Layout));
  std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicStateCI{};
  dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateCI.pDynamicStates = dynamicStateEnables.data();
  dynamicStateCI.dynamicStateCount =
      static_cast<uint32_t>(dynamicStateEnables.size());

	VkViewport DummyViewport = {0,0,1,1,0,1};
	VkRect2D Scissor = {0,0,1,1};

	VkPipelineViewportStateCreateInfo ViewportState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &DummyViewport,
		.scissorCount = 1,
		.pScissors = &Scissor
	};


  VkGraphicsPipelineCreateInfo PipelineInfo{};
  PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  PipelineInfo.stageCount = 2;
  PipelineInfo.pStages = ShaderStages;
  PipelineInfo.pVertexInputState = &VertexInputStateCreateInfo;
  PipelineInfo.pInputAssemblyState = &InputAssembly;
  PipelineInfo.pViewportState = &ViewportState;
  PipelineInfo.pRasterizationState = &RasterizationState;
  PipelineInfo.pMultisampleState = &Multisampling;
	PipelineInfo.pDepthStencilState = & DepthStencil;
  PipelineInfo.pColorBlendState = &ColorBlending;
  PipelineInfo.pDynamicState = &dynamicStateCI;
  PipelineInfo.layout = Layout;
  PipelineInfo.renderPass = Info.RenderPass->RenderPass;
  PipelineInfo.subpass = 0;
  PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  PipelineInfo.basePipelineIndex = -1;

  VK_CHECK(vkCreateGraphicsPipelines(*GRHI->GetDevice(), VK_NULL_HANDLE, 1, &PipelineInfo,
                                     nullptr, &Pipeline));


}

