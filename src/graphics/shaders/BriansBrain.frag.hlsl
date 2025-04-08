#include "Neighborhood.hlsli"

float4 main(float4 position : SV_Position) : SV_Target
{
	Neighborhood hood = getNeighborhood(position);

	if (hood.current == DEAD && hood.living == 2)
		return ALIVE;
	else if (hood.current == ALIVE)
		return DYING;
	else
		return DEAD;
}
