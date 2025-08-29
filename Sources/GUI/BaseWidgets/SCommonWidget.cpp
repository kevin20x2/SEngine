//
// Created by vinkzheng on 2025/8/29.
//

#include "SCommonWidget.h"

#include <imgui.h>

#include "Rendering/Mesh.h"


void SSliderFloatWidget::OnPaint()
{
    ImGui::SliderFloat(Name.c_str(),&Value ,MinValue,MaxValue);
    CheckValueChanged();
}

void SColorEditWidget::OnPaint()
{
    ImGui::ColorEdit3(Name.c_str(),(float *)&Value);
    CheckValueChanged();
}
