#include "Neighborhood.hlsli"

SamplerState Sampler : register(s0);

float4 main(float4 fragCoord : SV_Position) : SV_Target
{
	float state = Universe.Sample(Sampler, fragCoord.xy / float2(1920, 1080)).x;

	if (floatCmp(state, ALIVE))
		return float4(1, 1, 1, 1);
	else if (floatCmp(state, DYING))
		return float4(0.1, 0, 0, 1);
	else
		return float4(0, 0, 0, 1);
}
