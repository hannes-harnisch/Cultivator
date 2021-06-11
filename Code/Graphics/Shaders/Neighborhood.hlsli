#define ALIVE float4(1, 1, 1, 1)
#define DEAD  float4(0, 0, 0, 1)

Texture2D Universe : register(t0);

struct Neighborhood
{
	int current;
	int sum;
};

int getCell(float4 pos, int x, int y)
{
	return Universe.Load(pos.xyz, int2(x, y)).r;
}

Neighborhood getNeighborhood(float4 pos)
{
	Neighborhood hood;
	int nw		 = getCell(pos, -1, 1);
	int n		 = getCell(pos, 0, 1);
	int ne		 = getCell(pos, 1, 1);
	int w		 = getCell(pos, -1, 0);
	hood.current = getCell(pos, 0, 0);
	int e		 = getCell(pos, 1, 0);
	int sw		 = getCell(pos, -1, -1);
	int s		 = getCell(pos, 0, -1);
	int se		 = getCell(pos, 1, -1);
	hood.sum	 = nw + n + ne + w + hood.current + e + sw + s + se;
	return hood;
}
