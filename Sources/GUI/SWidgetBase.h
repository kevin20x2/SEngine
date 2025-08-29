//
// Created by vinkzheng on 2025/8/29.
//

#ifndef SWIDGETBASE_H
#define SWIDGETBASE_H
#include "CoreObjects/SObject.h"


class SWidgetBase : public SObject
{
public:
    virtual void OnPaint() = 0 ;

};



#endif //SWIDGETBASE_H
