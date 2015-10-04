texture2D baseTexture : register (t0);
texture2D secondTexture : register(t1);
SamplerState filters[2] : register(s0);

cbuffer POINTLIGHT : register(b0)
{
	float4 color;
	float3 position;
	float radius;
}
cbuffer SPOTLIGHT : register(b1)
{
	float4 colour;
	float3 pos;
	float ConeRadius;
	float3 ConeDir;
	float innerRatio;
	float outerRatio;
	float3 padding;
}
cbuffer DIRECTIONAL : register(b2)
{
	float4 dColour;
	float3 direction;
}
cbuffer MULTITEXTURES : register(b3)
{
	float4 multiples;
}
float4 main(float2 baseUV : UVS, float3 locals : POSITION, float3 norms : NORMALS) : SV_TARGET
{
	float4 baseColor = (baseTexture.Sample(filters[0], (float2)baseUV));
	float4 secondColor = float4(0, 0, 0, 0); 
	float4 final;

	if (multiples.x == 1)
	{
		secondColor = secondTexture.Sample(filters[0], (float2)baseUV);
		final = baseColor * secondColor;
	}
	else if (multiples.y == 1)
	{
		final = baseColor;
	}


	float3 lightDir = normalize(position - locals);
	float lightRatio = clamp(dot(lightDir, norms), 0 , 1);
	float attenu = 1.0f - clamp(length(position - locals) / radius, 0, 1);
	float4 pointLight = (attenu * lightRatio * color) * final;

		float3 spotLightDir = normalize(pos - locals);
		float surfaceRatio = clamp(dot(-spotLightDir, ConeDir), 0, 1);
	float radiusAtten = 1.0 - clamp(length(position - locals) / ConeRadius, 0, 1);
		float InnerOuterAtten = 1.0f - clamp((innerRatio - surfaceRatio) / (innerRatio - outerRatio), 0, 1);
		float spotLightRatio = clamp(dot(spotLightDir, norms), 0, 1);
		float4 spotLight = ((radiusAtten * InnerOuterAtten) * spotLightRatio * colour) * final;

			float directRatio = clamp(dot(-direction, norms), 0, 1);
		float4 directLight = directRatio * dColour * final;

			return spotLight + pointLight + directLight; 
}