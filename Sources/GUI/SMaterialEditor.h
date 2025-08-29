//
// Created by vinkzheng on 2025/8/29.
//

#ifndef SMATERIALEDITOR_H
#define SMATERIALEDITOR_H
#include "SWidgetBase.h"
#include "BaseWidgets/SCommonWidget.h"
#include "Rendering/Material.h"


class SMaterialEditor  : public SWidgetBase
{
    public:

    void Initialize(SMaterialInterface * Material);

    virtual void OnPaint() override;


    TArray < TSharedPtr < SWidgetBase> > ChildWidgets;

};



#endif //SMATERIALEDITOR_H
