#include "Neighborhood.hlsli"

float4 main(float4 position : SV_Position) : SV_Target
{
	Neighborhood hood = getNeighborhood(position);

	if(hood.living == 3)
		return ALIVE;
	else if(hood.living == 2 && hood.current == ALIVE)
		return ALIVE;
	else
		return DEAD;
}
