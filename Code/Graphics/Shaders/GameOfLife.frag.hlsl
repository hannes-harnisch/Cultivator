#include "Neighborhood.hlsli"

float4 main(float4 position : SV_Position) : SV_Target
{
	Neighborhood hood = getNeighborhood(position);

	if(hood.sum == 3)
		return ALIVE;
	else if(hood.sum == 4 && hood.current == 1)
		return ALIVE;
	else
		return DEAD;
}
