#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float3 coordinate : POSITION0;
	float3 uvs : UVS;
	float3 normals : NORMALS;
	float3 instancePos : POSITION1;
};

struct OUTPUT_VERTEX
{
	float2 uvsOut : UVS;
	float3 localSpace : POSITION;
	float3 normals : NORMALS;
	float4 colorOut : COLOR;
	float4 projectedCoordinate : SV_POSITION;
};

cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
};
cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectMatrix;
};

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{

	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	float4 localH = float4(fromVertexBuffer.coordinate, 1);


		localH = mul(localH, worldMatrix);

	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectMatrix);

	sendToRasterizer.projectedCoordinate = localH;
	sendToRasterizer.uvsOut = fromVertexBuffer.uvs.xy;
	sendToRasterizer.localSpace = fromVertexBuffer.coordinate;
	sendToRasterizer.normals = fromVertexBuffer.normals;
	return sendToRasterizer;
}