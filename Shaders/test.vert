
#include "SCommon.hlsl"

struct VSInput
{
    [[vk::location(0)]] float3 Pos : POSITION ;
    //[[vk::location(1)]] float4 Color : Color;
    [[vk::location(2)]] float4 Normal : NORMAL0;
    [[vk::location(3)]] float2 Uv : TEXCOORD0;
};


struct VSOutput
{
    float4 Pos : SV_POSITION;
    //[[vk::location(0)]] float4 Color : Color;
    [[vk::location(1)]] float4 Normal : NORMAL0;
    [[vk::location(2)]] float2 Uv : TEXCOORD0;
    [[vk::location(3)]] float4 WorldPos : TEXCOORD2;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput)0;
    float4 WorldPos = mul(float4(input.Pos.xyz,1.0f) , PrimitiveData.Model);
    output.Pos = mul(WorldPos,ViewData.ViewProjection);
    output.Normal = mul(float4( input.Normal.xyz,0.0f ) ,PrimitiveData.Model);
    output.WorldPos = WorldPos;
    output.Uv = input.Uv;
    return output;
}