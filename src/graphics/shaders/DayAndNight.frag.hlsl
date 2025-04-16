#include "Neighborhood.hlsli"

float4 main(float4 pos : SV_Position) : SV_Target
{
	Neighborhood hood = getNeighborhood(pos);

	if (hood.living == 3 || hood.living == 6 || hood.living == 7 || hood.living == 8)
		return ALIVE;
	else if (hood.current == ALIVE && hood.living == 4)
		return ALIVE;
	else
		return DEAD;
}
