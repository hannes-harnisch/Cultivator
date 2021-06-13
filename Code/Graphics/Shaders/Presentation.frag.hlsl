#include "Neighborhood.hlsli"

SamplerState Sampler : register(s0);

float4 main(float4 fragCoord : SV_Position) : SV_Target
{
	uint state = Universe.Sample(Sampler, fragCoord.xy / float2(1920, 1080)).x;
	switch(state)
	{
		case ALIVE: return float4(1, 1, 1, 1);
		case DYING: return float4(0, 1, 0, 1);
	}
	return float4(0, 0, 0, 1);
}
