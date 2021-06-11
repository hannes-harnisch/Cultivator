#include "Neighborhood.hlsli"

float4 main(float4 pos : SV_Position) : SV_Target
{
	Neighborhood hood = getNeighborhood(pos);

	if(hood.sum == 3 || hood.sum == 6 || hood.sum == 7 || hood.sum == 8)
		return ALIVE;
	else if(hood.current == 1 && hood.sum == 4)
		return ALIVE;
	else
		return DEAD;
}
