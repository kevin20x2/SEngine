//
// Created by 郑文凯 on 2024/10/1.
//

#include "Shader.h"
#include "volk.h"
#include "RHI/RHI.h"


void FShader::GenerateDescriptorSetLayout()
{
	if(VertexShaderProgram == nullptr || PixelShaderProgram == nullptr)
	{
		return ;
	}
	auto & VertexInfos = VertexShaderProgram->GetDescriptorSetLayoutInfos();
	auto & PixelInfos = PixelShaderProgram->GetDescriptorSetLayoutInfos();

	if(VertexInfos.size() != PixelInfos.size())
	{
		return ;
	}

	for(uint32 i = 0 ; i < VertexInfos.size(); ++ i)
	{
		TArray <VkDescriptorSetLayoutBinding> Bindings;
		VkDescriptorSetLayoutCreateInfo CreateInfo = VertexInfos[i].CreateInfo;
		for(auto & Binding : VertexInfos[i].Bindings)
		{
			Bindings.push_back(Binding);
		}

		for(auto & Binding : PixelInfos[i].Bindings)
		{
			Bindings.push_back(Binding);
		}

		CreateInfo.bindingCount = Bindings.size();
		CreateInfo.pBindings = Bindings.data();

		VK_CHECK(vkCreateDescriptorSetLayout(*GRHI->GetDevice(),
									&CreateInfo, nullptr,&DescriptorSetLayout));
	}
}
