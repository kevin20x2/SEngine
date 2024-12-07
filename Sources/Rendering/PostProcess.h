//
// Created by kevin on 2024/12/7.
//

#ifndef POSTPROCESS_H
#define POSTPROCESS_H
#include "Core/BaseTypes.h"
#include "RHI/RenderTargetGroup.h"


class SMaterialInterface;

class FPostProcess
{

    FString Name;

    TSharedPtr < SMaterialInterface > Material;
};



#endif //POSTPROCESS_H
