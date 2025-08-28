
#define PI 3.14159265358f

struct FViewData
{
    float4x4 ViewProjection;
    float3 ViewPos;
};

struct FPrimitiveData
{
    float4x4 Model;
};
struct FLightData
{
    float4 DirectionalLightDir;
    float4 DirectionalLightColorAndOpacity;
};

cbuffer ViewBuffer :register(b0)
{
    FViewData ViewData;
}
cbuffer PrimitiveBuffer : register(b1)
{
    FPrimitiveData PrimitiveData;
}

cbuffer LightBuffer : register(b2)
{
    FLightData LightData;
}


