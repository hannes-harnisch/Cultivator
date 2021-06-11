#include "Neighborhood.hlsli"

float4 main(float4 pos : SV_Position) : SV_Target
{
	Neighborhood hood = getNeighborhood(pos);

	if(hood.sum == 3)
		return ALIVE;
	else if(hood.current == 1)
		return ALIVE;
	else
		return DEAD;
}
