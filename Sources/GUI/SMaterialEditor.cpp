//
// Created by vinkzheng on 2025/8/29.
//

#include "SMaterialEditor.h"

#include <imgui.h>


void SMaterialEditor::Initialize(SMaterialInterface *Material)
{
    auto & Params = Material->GetMaterialParameters();

    for(auto Param : Params.Parameters)
    {
        if(auto Uniform =
            std::dynamic_pointer_cast<FMaterialParameterUniformBuffer>(Param))
        {
            auto & VariableInfos =  Uniform->GetVariableInfos();
            for(auto& [Name, VariableInfo] : VariableInfos)
            {
                if(VariableInfo.Type == EShaderVariableType::Float)
                {
                    auto SliderFloatWidget = std::make_shared<SSliderFloatWidget>();
                    SliderFloatWidget->SetName(Name);
                    Uniform->GetScalar(Name, SliderFloatWidget->Value);
                    SliderFloatWidget->OnValueChange.AddLambda([Name,Uniform](float ChangedValue)
                    {
                        Uniform->SetScalar(Name, ChangedValue);
                    });
                    ChildWidgets.Add(SliderFloatWidget);
                }
                if(VariableInfo.Type == EShaderVariableType::Vector)
                {
                    auto ColorEditWidget = std::make_shared<SColorEditWidget>();
                    ColorEditWidget->SetName(Name);
                    ColorEditWidget->OnValueChange.AddLambda([Name,Uniform](FVector Changed)
                    {
                        Uniform->SetVector(Name , FVector4(Changed,0.0) );
                    });
                    ChildWidgets.Add(ColorEditWidget);
                }
            }
        }

    }

}

void SMaterialEditor::OnPaint()
{
    ImGui::Begin("Material Editor");

    for(auto Child : ChildWidgets)
    {
        if(Child)
        {
            Child->OnPaint();
        }
    }
    ImGui::End();
}
