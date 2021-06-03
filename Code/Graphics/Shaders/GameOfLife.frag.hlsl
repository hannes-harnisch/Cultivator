#define ALIVE float4(1, 1, 1, 1)
#define DEAD  float4(0.5, 0, 0, 1)

Texture2D Universe;
SamplerState Sampler;

int getCell(float4 pos, int x, int y)
{
	return Universe.Sample(Sampler, pos.xy, int2(x, y)).r;
}

float4 main(float4 pos : SV_Position) : SV_Target
{
	int nw		= getCell(pos, -1, 1);
	int n		= getCell(pos, 0, 1);
	int ne		= getCell(pos, 1, 1);
	int w		= getCell(pos, -1, 0);
	int current = getCell(pos, 0, 0);
	int e		= getCell(pos, 1, 0);
	int sw		= getCell(pos, -1, -1);
	int s		= getCell(pos, 0, -1);
	int se		= getCell(pos, 1, -1);

	int sum = nw + n + ne + w + current + e + sw + s + se;
	if(sum == 3)
		return ALIVE;
	else if(sum == 4 && current == 1)
		return ALIVE;
	else
		return DEAD;
}
