struct GSOutput
{
	float2 uvs : UVS;
	float4 pos : SV_POSITION;
};
[maxvertexcount(4)]
void main(point float4 input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	GSOutput verts[4];
	verts[0].pos.x = -1;
	verts[0].pos.y = 0.75f;
	verts[0].pos.z = 0;
	verts[0].uvs = float2(0, 1);

	verts[1].pos.x = -1;
	verts[1].pos.y = 1;
	verts[1].pos.z = 0;
	verts[1].uvs = float2(0, 0);

	verts[2].pos.x = -0.75f;
	verts[2].pos.y = 0.75f;
	verts[2].pos.z = 0;
	verts[2].uvs = float2(1, 1);


	verts[3].pos.x = -0.75f;
	verts[3].pos.y = 1;
	verts[3].pos.z = 0;
	verts[3].uvs = float2(1, 0);

	for (uint i = 0; i < 4; i++)
	{
		verts[i].pos.w = 1;

		output.Append(verts[i]);
	}

	output.RestartStrip();
}