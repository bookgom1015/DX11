#ifndef _VIGNETTE
#define _VIGNETTE

#include "value.fx"

Texture2D<float4> gi_BackBuffer : register(t0);

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

VS_OUT VS_Vignette(in uint vid : SV_VertexID) {
    VS_OUT vout = (VS_OUT)0;                       
    vout.TexC = gVertices[vid];                         
    vout.PosH = float4(2.f * vout.TexC.x - 1.f, 1.f - 2.f * vout.TexC.y, 0.f, 1.f);
    
    return vout;                                         
}

float4 PS_Vignette(VS_OUT pin) : SV_Target {
    float3 scene = gi_BackBuffer.SampleLevel(g_sam_1, pin.TexC, 0).rgb;
    
    float2 uv = pin.TexC * 2.f - 1.f;
    float strength = 0.45f;
    float vignette = 1.f - dot(uv, uv) * strength;

    scene  *= saturate(vignette);
    
    return float4(scene, 1.f);
}

#endif // _VIGNETTE