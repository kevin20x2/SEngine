//
// Created by kevin on 2024/8/17.
//

#ifndef PRIMITIVECOMPONENT_H
#define PRIMITIVECOMPONENT_H
#include "SCoreComponentBase.h"
#include "Rendering/Mesh.h"


class SPrimitiveComponent : public SCoreComponentBase
{

protected:

    TSharedPtr<FStaticMesh > Mesh;
};

#endif //PRIMITIVECOMPONENT_H
