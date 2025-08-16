


float Hammersley(uint Index, uint NumSamples ,uint2 Random)
{
    float E1 = frac((float)Index / NumSamples + float(Random.x &0xffff)/(1<<16));
    float E2 = float(reversebits(Index) ^ Random.y) * 2.3283064365386963e-10;
    return float2(E1,E2);
}

float4 ImportanceSampleGGX(float2 E ,float a2)
{
    const float PI = 3.1415926f;
    float Phi = 2 * PI *E.x;
    float CosTheta = sqrt((1-E.y) / (1+(a2-1) *E.y));
    float SinTheta = sqrt(1 - CosTheta * CosTheta);

    float3 H;
    H.x = SinTheta * cos(Phi);
    H.y = SinTheta * sin(Phi);
    H.z = CosTheta;

    float d = (CosTheta * a2 -CosTheta) * CosTheta + 1;
    float D = a2 / (PI * d *d);
    float PDF = D * CosTheta;
    return float4(H,PDF);
}