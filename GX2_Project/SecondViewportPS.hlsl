texture2D baseTexture : register (t0);
SamplerState filters[2] : register(s0);
float4 main(float2 baseUV : UVS) : SV_TARGET
{
	float4 baseColor = baseTexture.Sample(filters[0], baseUV);
	return baseColor;
}