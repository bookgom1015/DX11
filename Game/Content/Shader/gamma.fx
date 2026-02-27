#ifndef _GAMMA
#define _GAMMA

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

VS_OUT VS_Gamma(in uint vid : SV_VertexID) {
    VS_OUT vout = (VS_OUT)0;                       
    vout.TexC = gVertices[vid];                         
    vout.PosH = float4(2.f * vout.TexC.x - 1.f, 1.f - 2.f * vout.TexC.y, 0.f, 1.f);
    
    return vout;                                         
}

float3 LinearToSRGB(float3 c) {
    float3 low  = c * 12.92;
    float3 high = 1.055 * pow(c, 1.0 / 2.4) - 0.055;
    return lerp(high, low, step(c, 0.0031308));
}

float4 PS_Gamma(VS_OUT pin) : SV_Target {
    float3 color = gi_BackBuffer.SampleLevel(g_sam_1, pin.TexC, 0).rgb;
    float3 colorCorrected = pow(color, 1.f / 2.2f);
    
    return float4(colorCorrected, 1.f);
}

#endif // _GAMMA