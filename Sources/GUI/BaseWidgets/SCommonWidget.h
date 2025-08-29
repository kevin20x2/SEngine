//
// Created by vinkzheng on 2025/8/29.
//

#ifndef SFLOATSLIDERWIDGET_H
#define SFLOATSLIDERWIDGET_H
#include <Delegates.h>

#include "GUI/SWidgetBase.h"
#include "Maths/Math.h"


template <typename ValueType>
class SCommonWidget : public SWidgetBase
{
public:

    void CheckValueChanged()
    {
        if(Value != ValueLastFrame)
        {
            OnValueChange.Broadcast(Value);
        }
    }

    MulticastDelegate<ValueType> OnValueChange;

    ValueType Value;
    ValueType ValueLastFrame;
};

class SSliderFloatWidget : public SCommonWidget<float>
{
public:
    virtual void OnPaint() override;

    float MinValue = 0;
    float MaxValue = 1.0;

};

class SColorEditWidget : public SCommonWidget<FVector>
{
public:
    virtual void OnPaint() override;

};


#endif //SFLOATSLIDERWIDGET_H
