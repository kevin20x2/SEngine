

struct VSInput
{
    [[vk::location(0)]] float3 Pos : POSITION ;
    //[[vk::location(1)]] float4 Color : Color;
    [[vk::location(2)]] float4 Normal : NORMAL0;
};

struct UBO
{
    
    float4x4 Mvp;
};


cbuffer ubo :register(b0) { UBO ubo; }


struct VSOutput
{
    float4 Pos : SV_POSITION;
    //[[vk::location(0)]] float4 Color : Color;
    [[vk::location(1)]] float4 Normal : NORMAL0;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput)0;
    //output.UV = input.UV;
    output.Pos = mul(float4(input.Pos.xyz,1.0f),ubo.Mvp);
    //output.Pos = float4(input.Pos.xy, 0.5f,1.0f);
    //output.Color = input.Color;
    output.Normal = input.Normal;
    return output;
}