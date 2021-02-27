float4 main(uint index : SV_VertexID) : SV_POSITION
{
	float x = (index << 1) & 2;
	float y = index & 2;
	return float4(x, y, 1, 1);
}
