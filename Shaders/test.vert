
struct VSInput
{
    [[vk::location(0)]] float3 Pos : POSITION ;
    //[[vk::location(1)]] float4 Color : Color;
    [[vk::location(2)]] float4 Normal : NORMAL0;
    [[vk::location(3)]] float2 Uv : TEXCOORD0;
};

struct FViewData
{
    float4x4 ViewProjection;
    float3 ViewPos;
};

struct FPrimitiveData
{
    float4x4 Model;
};

cbuffer ViewBuffer :register(b0) { FViewData ViewData; }
cbuffer PrimitiveBuffer : register(b1) { FPrimitiveData PrimitiveData; }


struct VSOutput
{
    float4 Pos : SV_POSITION;
    //[[vk::location(0)]] float4 Color : Color;
    [[vk::location(1)]] float4 Normal : NORMAL0;
    [[vk::location(2)]] float2 Uv : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput)0;
    float4 WorldPos = mul(float4(input.Pos.xyz,1.0f) , PrimitiveData.Model);
    output.Pos = mul(WorldPos,ViewData.ViewProjection);
    output.Normal = input.Normal;
    output.Uv = input.Uv;
    return output;
}