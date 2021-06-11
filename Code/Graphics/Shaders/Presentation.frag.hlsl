Texture2D Universe : register(t0);
SamplerState Sampler : register(s0);

float4 main(float4 fragCoord : SV_Position) : SV_Target
{
	return Universe.Sample(Sampler, fragCoord / float2(1920, 1080));
}
