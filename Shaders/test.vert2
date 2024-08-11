

struct VSInput
{
    [[vk::location(0)]] float3 Pos : POSITION ;
    [[vk::location(1)]] float2 UV : TEXCOORD0;
    [[vk::location(2)]] float3 Normal : NORMAL0;
};

struct UBO
{
    
    float4x4 Mvp;
};


cbuffer ubo :register(b0) { UBO ubo; }


struct VSOutput
{
    float4 Pos : SV_POSITION;
    [[vk::location(0)]] float2 UV :TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput)0;
    output.UV = input.UV;
    output.Pos = mul(ubo.Mvp, float4(input.Pos.xyz,1.0f));

    return output;
}