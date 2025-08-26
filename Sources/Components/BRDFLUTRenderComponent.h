//
// Created by vinkzheng on 2025/8/26.
//

#ifndef BRDFLUTRENDERCOMPONENT_H
#define BRDFLUTRENDERCOMPONENT_H
#include "CoreObjects/SCoreComponentBase.h"
#include "RHI/RenderTexture.h"


class SMaterialInterface;
class FRenderTargetGroup;

class SBRDFLUTRenderComponent : public SCoreComponentBase
{
    S_REGISTER_CLASS(SBRDFLUTRenderComponent);
public:

    void Init();

    void Render();

    TSharedPtr <FRenderTexture> GetRenderTexture();

protected:
    TSharedPtr  <FRenderTargetGroup> RTG;

    TSharedPtr <SMaterialInterface> Material;

};



#endif //BRDFLUTRENDERCOMPONENT_H
