

const float PI = 3.1415926f;

float Hammersley(uint Index, uint NumSamples ,uint2 Random)
{
    float E1 = frac((float)Index / NumSamples + float(Random.x &0xffff)/(1<<16));
    float E2 = float(reversebits(Index) ^ Random.y) * 2.3283064365386963e-10;
    return float2(E1,E2);
}

float4 ImportanceSampleGGX(float2 E ,float a2)
{
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

float Pow2(float x)
{
  return x*x;
}


// [ Duff et al. 2017, "Building an Orthonormal Basis, Revisited" ]
float3x3 GetTangentBasis( float3 TangentZ )
{
	const float Sign = TangentZ.z >= 0 ? 1 : -1;
	const float a = -rcp( Sign + TangentZ.z );
	const float b = TangentZ.x * TangentZ.y * a;

	float3 TangentX = { 1 + Sign * a * Pow2( TangentZ.x ), Sign * b, -Sign * TangentZ.x };
	float3 TangentY = { b,  Sign + a * Pow2( TangentZ.y ), -TangentZ.y };

	return float3x3( TangentX, TangentY, TangentZ );
}

