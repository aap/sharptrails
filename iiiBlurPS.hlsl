struct PS_INPUT
{
	float4 position		: POSITION;
	float3 texcoord0	: TEXCOORD0;
	float4 color		: COLOR0;
};

//uniform float3 contrastMult : register(c3);
//uniform float3 contrastAdd : register(c4);

sampler2D tex0 : register(s0);

float
scale(float low, float high, float x)
{
	return saturate(x/(high-low) - low/(high-low));
}

float4
main(PS_INPUT In) : COLOR
{
	float4 dst = tex2D(tex0, In.texcoord0.xy);

		//dst.a = 1.0;
		//dst.rgb = dst.rgb*contrastMult + contrastAdd;
		//return dst;

	float4 prev = dst;
	for(int i = 0; i < 5; i++){
		float4 tmp = dst*(1-In.color.a) + prev*In.color*In.color.a;
		prev = tmp;
	}

	return prev;
}
