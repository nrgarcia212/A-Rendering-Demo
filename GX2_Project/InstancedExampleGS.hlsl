#pragma pack_matrix(row_major)

struct GSOutput
{
	float2 uvs : UVS;
	float4 pos : SV_POSITION;
	float3 norms : NORMS;
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


[maxvertexcount(40)]
void main(point float4 input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	GSOutput verts[36];
	float size = 1.0f;

	//front face
	verts[0].pos = float4(input[0].x - size/2, input[0].y + size/2, input[0].z - size/2, 1);
	verts[0].uvs = float2(0, 0);
	verts[0].norms = float3(0, 0, -1);
	verts[1].pos = float4(input[0].x + size / 2, input[0].y + size / 2, input[0].z - size / 2, 1);
	verts[1].uvs = float2(1, 0);
	verts[1].norms = float3(0, 0, -1);
	verts[2].pos = float4(input[0].x - size / 2, input[0].y - size / 2, input[0].z - size / 2, 1);
	verts[2].uvs = float2(0, 1);
	verts[2].norms = float3(0, 0, -1);
	verts[3].pos = float4(input[0].x + size / 2, input[0].y + size / 2, input[0].z - size / 2, 1);
	verts[3].uvs = float2(1, 0);
	verts[3].norms = float3(0, 0, -1);
	verts[4].pos = float4(input[0].x + size / 2, input[0].y - size / 2, input[0].z - size / 2, 1);
	verts[4].uvs = float2(1, 1);
	verts[4].norms = float3(0, 0, -1);
	verts[5].pos = float4(input[0].x - size / 2, input[0].y - size / 2, input[0].z - size / 2, 1);
	verts[5].uvs = float2(0, 1);
	verts[5].norms = float3(0, 0, -1);

	//left face THIS IS NOT CODE!
	verts[6].pos = float4(input[0].x - size / 2, input[0].y + size / 2, input[0].z + size / 2, 1);
	verts[6].uvs = float2(0, 0);
	verts[6].norms = float3(-1, 0, 0);
	verts[7].pos = float4(input[0].x - size / 2, input[0].y + size / 2, input[0].z - size / 2, 1);
	verts[7].uvs = float2(1, 0);
	verts[7].norms = float3(-1, 0, 0);
	verts[8].pos = float4(input[0].x - size / 2, input[0].y - size / 2, input[0].z + size / 2, 1);
	verts[8].uvs = float2(0, 1);
	verts[8].norms = float3(-1, 0, 0);
	verts[9].pos = float4(input[0].x - size / 2, input[0].y + size / 2, input[0].z - size / 2, 1);
	verts[9].uvs = float2(1, 0);
	verts[9].norms = float3(-1, 0, 0);
	verts[10].pos = float4(input[0].x - size / 2, input[0].y - size / 2, input[0].z - size / 2, 1);
	verts[10].uvs = float2(1, 1);
	verts[10].norms = float3(-1, 0, 0);
	verts[11].pos = float4(input[0].x - size / 2, input[0].y - size / 2, input[0].z + size / 2, 1);
	verts[11].uvs = float2(0, 1);
	verts[11].norms = float3(-1, 0, 0);

	//right face
	verts[12].pos = float4(input[0].x + size / 2, input[0].y + size / 2, input[0].z - size / 2, 1);
	verts[12].uvs = float2(0, 0);
	verts[12].norms = float3(1, 0, 0);
	verts[13].pos = float4(input[0].x + size / 2, input[0].y + size / 2, input[0].z + size / 2, 1);
	verts[13].uvs = float2(1, 0);
	verts[13].norms = float3(1, 0, 0);
	verts[14].pos = float4(input[0].x + size / 2, input[0].y - size / 2, input[0].z - size / 2, 1);
	verts[14].uvs = float2(0, 1);
	verts[14].norms = float3(1, 0, 0);
	verts[15].pos = float4(input[0].x + size / 2, input[0].y - size / 2, input[0].z - size / 2, 1);
	verts[15].uvs = float2(0, 1);
	verts[15].norms = float3(1, 0, 0);
	verts[16].pos = float4(input[0].x + size / 2, input[0].y + size / 2, input[0].z + size / 2, 1);
	verts[16].uvs = float2(1, 0);
	verts[16].norms = float3(1, 0, 0);
	verts[17].pos = float4(input[0].x + size / 2, input[0].y - size / 2, input[0].z + size / 2, 1);
	verts[17].uvs = float2(1, 1);
	verts[17].norms = float3(1, 0, 0);

	//back face
	verts[18].pos = float4(input[0].x + size / 2, input[0].y + size / 2, input[0].z + size / 2, 1);
	verts[18].uvs = float2(0, 0);
	verts[18].norms = float3(0, 0, 1);
	verts[19].pos = float4(input[0].x - size / 2, input[0].y + size / 2, input[0].z + size / 2, 1);
	verts[19].uvs = float2(1, 0);
	verts[19].norms = float3(0, 0, 1);
	verts[20].pos = float4(input[0].x + size / 2, input[0].y - size / 2, input[0].z + size / 2, 1);
	verts[20].uvs = float2(0, 1);
	verts[20].norms = float3(0, 0, 1);
	verts[21].pos = float4(input[0].x + size / 2, input[0].y - size / 2, input[0].z + size / 2, 1);
	verts[21].uvs = float2(0, 1);
	verts[21].norms = float3(0, 0, 1);
	verts[22].pos = float4(input[0].x - size / 2, input[0].y + size / 2, input[0].z + size / 2, 1);
	verts[22].uvs = float2(1, 0);
	verts[22].norms = float3(0, 0, 1);
	verts[23].pos = float4(input[0].x - size / 2, input[0].y - size / 2, input[0].z + size / 2, 1);
	verts[23].uvs = float2(1, 1);
	verts[23].norms = float3(0, 0, 1);

	//top face
	verts[24].pos = float4(input[0].x - size / 2, input[0].y + size / 2, input[0].z + size / 2, 1);
	verts[24].uvs = float2(0, 0);
	verts[24].norms = float3(0, 1, 0);
	verts[25].pos = float4(input[0].x + size / 2, input[0].y + size / 2, input[0].z + size / 2, 1);
	verts[25].uvs = float2(1, 0);
	verts[25].norms = float3(0, 1, 0);
	verts[26].pos = float4(input[0].x - size / 2, input[0].y + size / 2, input[0].z - size / 2, 1);
	verts[26].uvs = float2(0, 1);
	verts[26].norms = float3(0, 1, 0);
	verts[27].pos = float4(input[0].x - size / 2, input[0].y + size / 2, input[0].z - size / 2, 1);
	verts[27].uvs = float2(0, 1);
	verts[27].norms = float3(0, 1, 0);
	verts[28].pos = float4(input[0].x + size / 2, input[0].y + size / 2, input[0].z + size / 2, 1);
	verts[28].uvs = float2(1, 0);
	verts[28].norms = float3(0, 1, 0);
	verts[29].pos = float4(input[0].x + size / 2, input[0].y + size / 2, input[0].z - size / 2, 1);
	verts[29].uvs = float2(1, 1);
	verts[29].norms = float3(0, 1, 0);

	//back face
	verts[30].pos = float4(input[0].x - size / 2, input[0].y - size / 2, input[0].z - size / 2, 1);
	verts[30].uvs = float2(0, 0);
	verts[30].norms = float3(0, -1, 0);
	verts[31].pos = float4(input[0].x + size / 2, input[0].y - size / 2, input[0].z - size / 2, 1);
	verts[31].uvs = float2(1, 0);
	verts[31].norms = float3(0, -1, 0);
	verts[32].pos = float4(input[0].x - size / 2, input[0].y - size / 2, input[0].z + size / 2, 1);
	verts[32].uvs = float2(0, 1);
	verts[32].norms = float3(0, -1, 0);
	verts[33].pos = float4(input[0].x - size / 2, input[0].y - size / 2, input[0].z + size / 2, 1);
	verts[33].uvs = float2(0, 1);
	verts[33].norms = float3(0, -1, 0);
	verts[34].pos = float4(input[0].x + size / 2, input[0].y - size / 2, input[0].z - size / 2, 1);
	verts[34].uvs = float2(1, 0);
	verts[34].norms = float3(0, -1, 0);
	verts[35].pos = float4(input[0].x + size / 2, input[0].y - size / 2, input[0].z + size / 2, 1);
	verts[35].uvs = float2(1, 1);
	verts[35].norms = float3(0, -1, 0);


	for (uint i = 0; i < 36; i++)
	{
		if (i % 3 == 0)
		{
		
			output.RestartStrip();
		}
		
		verts[i].pos = mul(verts[i].pos, worldMatrix);
		verts[i].pos = mul(verts[i].pos, viewMatrix);
		verts[i].pos = mul(verts[i].pos, projectMatrix);
		output.Append(verts[i]);		
	}

}