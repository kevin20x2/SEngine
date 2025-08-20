//
// Created by vinkzheng on 2025/8/20.
//

#ifndef IRRADIANCECUBECOMPONENT_H
#define IRRADIANCECUBECOMPONENT_H
#include "CoreObjects/SCoreComponentBase.h"
#include "CoreObjects/SObject.h"


class SMaterialInterface;
class FRenderTargetCube;

class SIrradianceCubeComponent : public SCoreComponentBase
{
    S_REGISTER_CLASS(SIrradianceCubeComponent)
public:
    void Init();

    void SetCubeRT(TSharedPtr <FRenderTargetCube> InCubeRT)
    {
        CubeRT = InCubeRT;
    }
    void GenerateIrradianceCubeMap();
protected:
    TSharedPtr <FRenderTargetCube> CubeRT;

    TSharedPtr <SMaterialInterface> Material;
};



#endif //IRRADIANCECUBECOMPONENT_H
