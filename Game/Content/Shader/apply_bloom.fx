#ifndef _APPLY_BLOOM
#define _APPLY_BLOOM

#include "value.fx"

Texture2D<float4> gi_BackBuffer	: register(t0);
Texture2D<float4> gi_BloomMap	: register(t1);

static const float2 gVertices[6] = {
	float2(0.f, 1.f),
	float2(0.f, 0.f),
	float2(1.f, 0.f),
	float2(0.f, 1.f),
	float2(1.f, 0.f),
	float2(1.f, 1.f)
};

struct VS_OUT {
    float4 PosH : SV_Position; 
    float2 TexC : TEXCOORD;
};

VS_OUT VS_Bloom(in uint vid : SV_VertexID) {
    VS_OUT vout = (VS_OUT)0;                       
    vout.TexC = gVertices[vid];                         
    vout.PosH = float4(2.f * vout.TexC.x - 1.f, 1.f - 2.f * vout.TexC.y, 0.f, 1.f);
    
    return vout;                                         
}

float3 SoftAddBloom(in float3 hdr, in float3 bloom) {
	return lerp(hdr, hdr + bloom, 0.7f);
}

float4 PS_Bloom(VS_OUT pin) : SV_Target {
    const float3 Scene = gi_BackBuffer.SampleLevel(g_sam_0, pin.TexC, 0).rgb;
	const float3 Bloom = gi_BloomMap.SampleLevel(g_sam_0, pin.TexC, 0).rgb;
	
	const float3 Color = SoftAddBloom(Scene, Bloom);
	
	return float4(Color, 1.f);
}

#endif // _APPLY_BLOOM