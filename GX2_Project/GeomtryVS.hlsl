#pragma pack_matrix(row_major)
struct INPUT_VERTEX
{
	float3 coordinate : POSITION;
};
struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
};
OUTPUT_VERTEX main( INPUT_VERTEX input )
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	float4 localH = float4(input.coordinate, 1);

		sendToRasterizer.projectedCoordinate = localH;

	return sendToRasterizer;
}