
#include "SCommon.hlsl"

Texture2D texColor : register(t3 ,space0);
SamplerState samplerColor : register(s4,space0);


struct VSOutput
{
    float4 Pos : SV_POSITION;
    //[[vk::location(0)]] float4 Color :Color;
    [[vk::location(1)]] float4 Normal :NORMAL;
    [[vk::location(2)]] float2 Uv : TEXCOORD0;
    [[vk::location(3)]] float4 WorldPos : TEXCOORD2;
};

float4 main(VSOutput input) :SV_TARGET
{
    float4 ao = texColor.Sample(samplerColor,float2(input.Uv.x,1.0f - input.Uv.y));
    float3 ViewDir = normalize( ViewData.ViewPos - input.WorldPos.xyz);
    float3 WorldNormal = normalize(input.Normal);
    float NToL = max(0.0f , dot(WorldNormal , LightData.DirectionalLightDir));
    float3 LambertDiffuse = pow(NToL * 0.5f + 0.5f,2.0f) * ao;
    float3 FinalColor = LambertDiffuse * LightData.DirectionalLightColorAndOpacity.xyz ;
    return float4(FinalColor,1.0f) ;
}