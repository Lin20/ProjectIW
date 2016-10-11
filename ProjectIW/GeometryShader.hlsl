

struct VSOutput
{
	float4 pos : SV_POSITION;
	float3 norm : TEXCOORD0;
	float3 norm_flat : TEXCOORD1;
	float3 color : COLOR0;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float3 norm : TEXCOORD0;
	float3 norm_flat : TEXCOORD1;
	float3 color : COLOR0;
};

[maxvertexcount(3)]
void main(triangle VSOutput input[3], inout TriangleStream< GSOutput > output)
{
	/* face normal */
	float3 ea = input[1].pos - input[0].pos;
	float3 eb = input[2].pos - input[0].pos;
	float3 face_normal = normalize(cross(eb, ea));
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element = input[i];

		const float C = 1.0f;
		const float far = 10000.0f * 1000.0f;
		const float FC = 1.0f / log(far*C + 1);
		element.pos.z = log(element.pos.w*C + 1)*FC * element.pos.w;

		element.norm_flat = face_normal;
		output.Append(element);
	}
}