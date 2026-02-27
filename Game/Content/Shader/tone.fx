#ifndef _TONE
#define _TONE

#include "value.fx"

#define Disabled g_int_0
#define ToneType g_int_1

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

VS_OUT VS_Tone(in uint vid : SV_VertexID) {
    VS_OUT vout = (VS_OUT)0;                       
    vout.TexC = gVertices[vid];                         
    vout.PosH = float4(2.f * vout.TexC.x - 1.f, 1.f - 2.f * vout.TexC.y, 0.f, 1.f);
    
    return vout;                                         
}

float3 TonemapACES(in float3 hdr) {
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return saturate((hdr * (a * hdr + b)) / (hdr * (c * hdr + d) + e));
}

float3 HorrorTone(float3 color) {
    // 1. 전체 채도 감소
    float luminance = dot(color, float3(0.2126, 0.7152, 0.0722));
    color = lerp(luminance.xxx, color, 0.2); // 0.4~0.6 추천

    // 2. 그림자 쿨톤화
    float shadowMask = saturate(1.0 - luminance * 2.0);
    color += float3(0.0, 0.02, 0.05) * shadowMask;

    // 3. 약한 콘트라스트 재조정
    color = pow(color, 1.5);

    return color;
}


float3 AnimeTone(float3 color) {
    float lum = dot(color, float3(0.299,0.587,0.114));

    // 강한 채도
    color = lerp(lum.xxx, color, 1.5);

    // 블랙 리프트
    color = max(color, 0.05);

    // 하이라이트 부드럽게
    color = smoothstep(0.0, 1.0, color);

    // 색 대비 강화
    float3 boosted;
    boosted.r = color.r * 1.1;
    boosted.g = color.g * 1.15;
    boosted.b = color.b * 1.2;

    return saturate(boosted);
}

float4 PS_Tone(VS_OUT pin) : SV_Target {    
    float3 hdr = gi_BackBuffer.SampleLevel(g_sam_1, pin.TexC, 0).rgb;    
    if (Disabled) return float4(hdr, 1.f);
    
    float3 color = hdr * 1.4f;
    
    float3 sdr = TonemapACES(color);
    
    if (ToneType == 1)
        sdr = HorrorTone(sdr);    
    else if (ToneType == 2)
        sdr = AnimeTone(sdr);    
    
    return float4(sdr, 1.f);
}

#endif // _TONE