#define ALIVE float4(1, 1, 1, 1)
#define DEAD  float4(0, 0, 0, 1)

Texture2D Universe;
SamplerState Sampler;

float2 VPos : VPOS;

int getCell(int x, int y)
{
	return Universe.Sample(Sampler, VPos, int2(x, y)).r;
}

float4 main() : SV_TARGET
{
	int nw		= getCell(-1, 1);
	int n		= getCell(0, 1);
	int ne		= getCell(1, 1);
	int w		= getCell(-1, 0);
	int current = getCell(0, 0);
	int e		= getCell(1, 0);
	int sw		= getCell(-1, -1);
	int s		= getCell(0, -1);
	int se		= getCell(1, -1);

	int sum = nw + n + ne + w + current + e + sw + s + se;
	if(sum == 3)
		return ALIVE;
	else if(sum == 4 && current == 1)
		return ALIVE;
	else
		return DEAD;
}
