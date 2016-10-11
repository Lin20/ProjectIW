cbuffer WVP : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
	float3 norm : TEXCOORD0;
	float3 norm_flat : TEXCOORD1;
	float3 color : COLOR0;
};

static float3 light_dir = float3(0.4f, 1.0f, 0.1f);
static float3 brown = float3(0.4375f, 0.375f, 0.25f);

VSOutput main(float4 pos : POSITION, float3 norm : NORMAL0, float4 color : COLOR0)
{
	VSOutput output;
	float4 world_pos = mul(pos, world);
	float4 view_pos = mul(world_pos, view);
	output.pos = mul(view_pos, proj);
	/*const float C = 0.1f;
	const float far = 10000.0f * 1000.0f;
	output.pos.z = log(C*output.pos.w + 1) / log(C*far + 1) * output.pos.w;*/

	output.norm = normalize(norm);
	output.norm_flat = output.norm;
	output.color = color;
	return output;
}