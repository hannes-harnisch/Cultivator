float4 main(uint index : SV_VertexID) : SV_Position
{
	float2 outUV = float2((index << 1) & 2, index & 2);
	return float4(2 * outUV - 1, 0, 1);
}
