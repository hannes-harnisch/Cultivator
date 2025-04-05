#include "Neighborhood.hlsli"

float4 main(float4 pos : SV_Position) : SV_Target
{
	Neighborhood hood = getNeighborhood(pos);

	if(hood.current == DEAD && hood.living == 2)
		return ALIVE;
	else
		return DEAD;
}
