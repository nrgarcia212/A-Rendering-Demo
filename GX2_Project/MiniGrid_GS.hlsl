#pragma pack_matrix(row_major)

struct GSOutput
{
	float2 uvs : UVS;
	float4 pos : SV_POSITION;
};
cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
}
cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectMatrix;
}

[maxvertexcount(4)]
void main(point float4 input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	GSOutput verts[4];
	float size = 10.0f;

	verts[0].pos.x = input[0].x - size/2;
	verts[0].pos.y = input[0].y - size/2;
	verts[0].pos.z = input[0].z;
	verts[0].uvs = float2(0, 1);

	verts[1].pos.x = input[0].x - size/2;
	verts[1].pos.y = input[0].y + size/2;
	verts[1].pos.z = input[0].z;
	verts[1].uvs = float2(0, 0);

	verts[2].pos.x = input[0].x + size/2;
	verts[2].pos.y = input[0].y - size/2;
	verts[2].pos.z = input[0].z;
	verts[2].uvs = float2(1, 1);

	
	verts[3].pos.x = input[0].x + size / 2;
	verts[3].pos.y = input[0].y + size / 2;
	verts[3].pos.z = input[0].z;
	verts[3].uvs = float2(1, 0);



	for (uint i = 0; i < 4; i++)
	{
		verts[i].pos.w = 1;

		verts[i].pos = mul(verts[i].pos, worldMatrix);
		verts[i].pos = mul(verts[i].pos, viewMatrix);
		verts[i].pos = mul(verts[i].pos, projectMatrix);
		
		output.Append(verts[i]);
	}

	output.RestartStrip();
}