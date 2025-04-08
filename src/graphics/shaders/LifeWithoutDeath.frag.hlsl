#include "Neighborhood.hlsli"

float4 main(float4 pos : SV_Position) : SV_Target
{
	Neighborhood hood = getNeighborhood(pos);

	if (hood.living == 3)
		return ALIVE;
	else if (hood.current == ALIVE)
		return ALIVE;
	else
		return DEAD;
}
