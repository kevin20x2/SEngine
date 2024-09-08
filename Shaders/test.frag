

Texture2D texColor : register(t1 ,space0);
SamplerState samplerColor : register(s1,space0);


struct VSOutput
{
    float4 Pos : SV_POSITION;
    [[vk::location(0)]] float4 Color :Color;
    [[vk::location(1)]] float4 Normal :NORMAL;
};

float4 main(VSOutput input) :SV_TARGET
{
    //float4 color = texColor.Sample(samplerColor,input.UV);
    return input.Normal;
}