textureCUBE baseTexture : register (t0);
SamplerState filters : register(s0);


cbuffer TIME : register(b0)
{
	float4 timer;
};
float4 main(float4 localSpace : SV_POSITION, float3 textureCoord : TEXTCOORD) : SV_TARGET
{
	
	textureCoord.x += sin(textureCoord.y * 200 + (timer.x * 5)) * 0.005 * (textureCoord.y - 1);
	
		
	float4 baseColor = baseTexture.Sample(filters, textureCoord);

		return baseColor;
}