#define ALIVE float4(1, 1, 1, 1)
#define DEAD  float4(0, 0, 0, 1)

Texture2D Universe;
SamplerState Sampler;

float2 VPos : VPOS;

int getCell(int2 offset)
{
	return Universe.Sample(Sampler, VPos, offset).r;
}

float4 main() : SV_TARGET
{
	int nw		= getCell(int2(-1, 1));
	int n		= getCell(int2(0, 1));
	int ne		= getCell(int2(1, 1));
	int w		= getCell(int2(-1, 0));
	int current = getCell(int2(0, 0));
	int e		= getCell(int2(1, 0));
	int sw		= getCell(int2(-1, -1));
	int s		= getCell(int2(0, -1));
	int se		= getCell(int2(1, -1));

	int sum = nw + n + ne + w + current + e + sw + s + se;
	if(sum == 3)
		return ALIVE;
	else if(sum == 4 && current == 1)
		return ALIVE;
	else
		return DEAD;
}
