static float3 light_dir = float3(0.4f, 1.0f, 0.1f);
static float3 brown = float3(0.4375f, 0.375f, 0.25f);

struct VSOutput
{
	float4 pos : SV_POSITION;
	float3 norm : TEXCOORD0;
	nointerpolation float3 norm_flat : TEXCOORD1;
	float3 color : COLOR0;
};

float4 main(VSOutput input) : SV_TARGET
{
	float d = dot(normalize(light_dir), normalize(input.norm_flat));
	d = (d + 1.0f) * 0.5f;
	float m = lerp(0.6f, 1, d);

	d = dot(normalize(light_dir), input.norm);
	d = (d + 1.0f) * 0.5f;
	float m2 = lerp(-0.1f, 0.15f, d);
	float m3 = lerp(0.6f, 1, d);

	//float3 base_color = brown;
	//base_color *= m;
	float3 base_color = (normalize(input.norm) * 0.5f + float3(.5, .5, .5)).xyz;
	float3 sun_color = float3(1.0f, 1.0f, 0.6f);
	sun_color *= m2;

	//return float4(input.color.xyz, 1.0f);
	return float4(base_color, 1.0f);
}