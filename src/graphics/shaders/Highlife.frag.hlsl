#include "Neighborhood.hlsli"

float4 main(float4 pos : SV_Position) : SV_Target
{
	Neighborhood hood = getNeighborhood(pos);

	if(hood.living == 3 || hood.living == 6)
		return ALIVE;
	else if(hood.current == ALIVE && hood.living == 2)
		return ALIVE;
	else
		return DEAD;
}
