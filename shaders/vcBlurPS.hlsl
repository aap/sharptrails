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
	//float4 color = tex2D(tex0, In.texcoord0.xy) * 0.25;
	//float2 dist = float2(0.005, 0.005);
	//color += tex2D(tex0, In.texcoord0.xy + dist) * 0.175;
	//color += tex2D(tex0, In.texcoord0.xy - dist) * 0.175;
	//color += tex2D(tex0, In.texcoord0.xy + float2(dist.x, -dist.y)) * 0.2;
	//color += tex2D(tex0, In.texcoord0.xy + float2(-dist.x, dist.y)) * 0.2;
	//return color;

	float a = 30/255.0f;
	float4 doublec = saturate(In.color*2);
	float4 dst = tex2D(tex0, In.texcoord0.xy);
	float4 prev = dst;
	for(int i = 0; i < 5; i++){
		float4 tmp = dst*(1-a) + prev*doublec*a;
		tmp += prev*In.color;
		tmp += prev*In.color;
		prev = saturate(tmp);
	}
	return prev;
}
