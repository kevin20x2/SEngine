
Texture2D texColor : register(t2 ,space0);
SamplerState samplerColor : register(s3,space0);


struct VSOutput
{
    float4 Pos : SV_POSITION;
    //[[vk::location(0)]] float4 Color :Color;
    [[vk::location(1)]] float4 Normal :NORMAL;
    [[vk::location(2)]] float2 Uv : TEXCOORD0;
};

float4 main(VSOutput input) :SV_TARGET
{
    float4 color = texColor.Sample(samplerColor,input.Uv);
    //return float4(input.Uv, 0.0f,1.0f);
    return color;
}