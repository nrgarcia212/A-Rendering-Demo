#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float3 coordinate : POSITION;
	float3 uvs : UVS;
	float3 normals : NORMALS;
};
struct OUTPUT_VERTEX
{
	float4 localSpace : SV_POSITION;
	float3 textureCoord : TEXTCOORD;
};

cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
};
cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectMatrix;
}
OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	float4 localPos = float4(fromVertexBuffer.coordinate, 1);
	float4 worldPos = mul(localPos, worldMatrix);
	float4 viewPos = mul(worldPos, viewMatrix);
	sendToRasterizer.localSpace = mul(viewPos, projectMatrix);

	float4 verts = mul(localPos, worldMatrix);
		sendToRasterizer.textureCoord = normalize(fromVertexBuffer.coordinate);

	return sendToRasterizer;
}