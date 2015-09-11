struct PS_INPUT
{
	float4 position		: POSITION;
	float3 texcoord0	: TEXCOORD0;
	float4 color		: COLOR0;
};

sampler2D tex0 : register(s0);

float4
main(PS_INPUT In) : COLOR
{
	float4 dst = tex2D(tex0, In.texcoord0.xy);
	float4 prev = dst;
	for(int i = 0; i < 5; i++){
		float4 tmp = dst*(1-In.color.a) + prev*In.color*In.color.a;
		prev = tmp;
	}
	return prev;
}
