#define DEAD  0
#define ALIVE 1
#define DYING 2

Texture2D Universe : register(t0);

struct Neighborhood
{
	uint current;
	uint living;
};

uint getCell(float4 pos, int x, int y)
{
	return Universe.Load(int3(pos.xy, 0), int2(x, y)).x;
}

Neighborhood getNeighborhood(float4 pos)
{
	Neighborhood hood;
	hood.current = Universe.Load(int3(pos.xy, 0)).x;
	hood.living	 = 0;

	uint sw = getCell(pos, -1, -1);
	uint s	= getCell(pos, 0, -1);
	uint se = getCell(pos, 1, -1);
	uint w	= getCell(pos, -1, 0);
	uint e	= getCell(pos, 1, 0);
	uint nw = getCell(pos, -1, 1);
	uint n	= getCell(pos, 0, 1);
	uint ne = getCell(pos, 1, 1);

	if(sw == ALIVE)
		hood.living++;
	if(s == ALIVE)
		hood.living++;
	if(se == ALIVE)
		hood.living++;
	if(w == ALIVE)
		hood.living++;
	if(e == ALIVE)
		hood.living++;
	if(nw == ALIVE)
		hood.living++;
	if(n == ALIVE)
		hood.living++;
	if(ne == ALIVE)
		hood.living++;

	return hood;
}
