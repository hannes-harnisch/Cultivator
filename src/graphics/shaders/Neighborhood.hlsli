#define DEAD  0
#define DYING 0.5
#define ALIVE 1.0

Texture2D Universe : register(t0);

struct Neighborhood
{
	uint current;
	uint living;
};

bool floatCmp(float a, float b)
{
	return abs(a - b) < 1e-1;
}

uint getCell(float4 pos, int x, int y)
{
	return Universe.Load(int3(pos.xy + int2(x, y), 0)).x;
}

Neighborhood getNeighborhood(float4 pos)
{
	Neighborhood hood;
	hood.current = Universe.Load(int3(pos.xy, 0)).x;
	hood.living = 0;

	uint sw = getCell(pos, -1, -1);
	uint ss = getCell(pos, 0, -1);
	uint se = getCell(pos, 1, -1);
	uint ww = getCell(pos, -1, 0);
	uint ee = getCell(pos, 1, 0);
	uint nw = getCell(pos, -1, 1);
	uint nn = getCell(pos, 0, 1);
	uint ne = getCell(pos, 1, 1);

	if (sw == ALIVE)
		hood.living++;
	if (ss == ALIVE)
		hood.living++;
	if (se == ALIVE)
		hood.living++;
	if (ww == ALIVE)
		hood.living++;
	if (ee == ALIVE)
		hood.living++;
	if (nw == ALIVE)
		hood.living++;
	if (nn == ALIVE)
		hood.living++;
	if (ne == ALIVE)
		hood.living++;

	return hood;
}
