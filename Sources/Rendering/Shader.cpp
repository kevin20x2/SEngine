//
// Created by 郑文凯 on 2024/10/1.
//

#include "Shader.h"

#include "Material.h"
#include "volk.h"
#include "Core/Log.h"
#include "RHI/RHI.h"


void SShader::AddUsedMaterial(SMaterialInterface *MaterialInterface)
{
    UsedMaterials.AddUnique(MaterialInterface->AsShared());
}

void SShader::RemoveUsedMaterial(SMaterialInterface *MaterialInterace)
{
    UsedMaterials.Remove(MaterialInterace->AsShared());
}

void SShader::OnReCompile()
{
    if (VertexShaderProgram)
    {
        VertexShaderProgram->ReCompile();
    }
    if (PixelShaderProgram)
    {
        PixelShaderProgram->ReCompile();
    }
    for (auto Material: UsedMaterials)
    {
        if (Material)
        {
            Material->MarkDirty();
        }
    }
}

void SShader::GenerateDescriptorSetLayout()
{
    if (VertexShaderProgram == nullptr || PixelShaderProgram == nullptr)
    {
        return;
    }
    auto &VertexInfos = VertexShaderProgram->GetDescriptorSetLayoutInfos();
    auto &PixelInfos = PixelShaderProgram->GetDescriptorSetLayoutInfos();

    if (VertexInfos.size() != PixelInfos.size())
    {
        return;
    }

    for (uint32 i = 0; i < VertexInfos.size(); ++i)
    {
        TArray<VkDescriptorSetLayoutBinding> Bindings;
        TArray<VkDescriptorBindingFlags> BindingFlags;
        VkDescriptorSetLayoutCreateInfo CreateInfo = {}; // = VertexInfos[i].CreateInfo;
        CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        VkDescriptorSetLayoutBindingFlagsCreateInfo BindingFlagCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
        };
        //CreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
        for (auto &Binding: VertexInfos[i].Bindings)
        {
            if (Bindings.FindByPredict([&](const VkDescriptorSetLayoutBinding &It)
            {
                return It.binding == Binding.Binding.binding;
            }) == Bindings.end())
            {
                Bindings.push_back(Binding.Binding);
                BindingFlags.push_back(VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
            }
        }

        for (auto &Binding: PixelInfos[i].Bindings)
        {
            if (auto Iter = Bindings.FindByPredict([&](const VkDescriptorSetLayoutBinding &It)
            {
                return It.binding == Binding.Binding.binding;
            }); Iter == Bindings.end())
            {
                Bindings.push_back(Binding.Binding);
                BindingFlags.push_back(VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
            } else
            {
                Iter->stageFlags |= Binding.Binding.stageFlags;
            }
        }

        BindingFlagCreateInfo.bindingCount = BindingFlags.size();
        BindingFlagCreateInfo.pBindingFlags = BindingFlags.data();

        CreateInfo.pNext = &BindingFlagCreateInfo;
        CreateInfo.bindingCount = Bindings.size();
        CreateInfo.pBindings = Bindings.data();

        VK_CHECK(vkCreateDescriptorSetLayout(*GRHI->GetDevice(),
            &CreateInfo, nullptr,&DescriptorSetLayout));
    }
}

static void HandleImageParams(const FDescriptorSetLayoutBinding &Binding, uint32 DescriptorIdx,
                              FMaterialParameters &MaterialParams)
{
    auto &Texture = MaterialParams.Parameters.emplace_back(TSharedPtr<FMaterialParameterTexture>(
        new FMaterialParameterTexture()
    ));
    Texture->Name = Binding.Name;
    Texture->DescriptorIdx = DescriptorIdx;
    Texture->BindingSlotIdx = Binding.Binding.binding;
}

static void HandleSamplerParams(const FDescriptorSetLayoutBinding &Binding, uint32 DescriptorIdx,
                              FMaterialParameters &MaterialParams)
{
    VkFilter FilterType = VK_FILTER_LINEAR;
    FString Name = Binding.Name;
    std::transform(Name.begin(), Name.end(), Name.begin(), [](unsigned char c)
    {
        return std::toupper(c);
    });
    if (Name.find("POINT") != std::string::npos)
    {
        FilterType = VK_FILTER_NEAREST;
    }
    auto &Sampler = MaterialParams.Parameters.emplace_back(TSharedPtr<FMaterialParameterSampler>(
        new FMaterialParameterSampler(FilterType)
    ));
    Sampler->Name = Binding.Name;
    Sampler->DescriptorIdx = DescriptorIdx;
    Sampler->BindingSlotIdx = Binding.Binding.binding;
}

static void HandleUniformParams(const FDescriptorSetLayoutBinding &Binding, uint32 DescriptorIdx,
                              FMaterialParameters &MaterialParams,SMaterialInterface * MaterialInterface)
{
    auto &Uniform = MaterialParams.Parameters.emplace_back(
        new FMaterialParameterUniformBuffer(MaterialInterface, Binding));
    Uniform->Name = Binding.Name;
    Uniform->DescriptorIdx = DescriptorIdx;
    Uniform->BindingSlotIdx = Binding.Binding.binding;

}


void
SShader::GenerateDefaultMaterialParams(FMaterialParameters &MaterialParams, SMaterialInterface *MaterialInterface)
{
    auto BindingListList = GenerateLayoutBindings();
    uint32 DescriptorIdx = 0;
    for (auto &BindingList: BindingListList)
    {
        for (auto &Binding: BindingList)
        {
            switch(Binding.Binding.descriptorType)
            {
                case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                    HandleImageParams(Binding,DescriptorIdx,MaterialParams);
                break;
                case VK_DESCRIPTOR_TYPE_SAMPLER:
                    HandleSamplerParams(Binding,DescriptorIdx,MaterialParams);
                break;
                case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                    HandleUniformParams(Binding,DescriptorIdx,MaterialParams,MaterialInterface);
                break;
                default:
                    SLogW(TEXT("unknown descriptor Type.."));
            }

        }
        DescriptorIdx++;
    }
}

TArray<SShader::DescriptorSetLayoutBindingList>
SShader::GenerateLayoutBindings()
{
    if (VertexShaderProgram == nullptr || PixelShaderProgram == nullptr)
    {
        return {};
    }
    auto &VertexInfos = VertexShaderProgram->GetDescriptorSetLayoutInfos();
    auto &PixelInfos = PixelShaderProgram->GetDescriptorSetLayoutInfos();

    if (VertexInfos.size() != PixelInfos.size())
    {
        return {};
    }

    TArray<SShader::DescriptorSetLayoutBindingList> Results;

    for (uint32 i = 0; i < VertexInfos.size(); ++i)
    {
        TArray<FDescriptorSetLayoutBinding> Bindings;
        for (auto &Binding: VertexInfos[i].Bindings)
        {
            Bindings.push_back(Binding);
        }


        Results.push_back(Bindings);
    }
    return Results;
}
