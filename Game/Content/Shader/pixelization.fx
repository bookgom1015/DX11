#ifndef _PIXEL
#define _PIXEL

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

VS_OUT VS_Pixel(in uint vid : SV_VertexID) {
    VS_OUT vout = (VS_OUT)0;                       
    vout.TexC = gVertices[vid];                         
    vout.PosH = float4(2.f * vout.TexC.x - 1.f, 1.f - 2.f * vout.TexC.y, 0.f, 1.f);
    
    return vout;                                         
}

float4 PS_Pixel(VS_OUT pin) : SV_Target {
	uint2 size;
	gi_BackBuffer.GetDimensions(size.x, size.y);
    
    uint2 indices = pin.TexC * size - 0.5f;
	
	const uint pixelSize = 6;

	float2 p  = pin.TexC * size;
	float2 b  = floor(p / pixelSize);
	float2 f  = frac(p / pixelSize);        // 0..1 inside block
	
	float2 pc = (b * pixelSize) + (pixelSize * 0.5f);
	float2 texcC = pc / size;
	
	// 이웃 블록 중심(오른쪽/아래)도 샘플해서 경계에서만 살짝 섞기
	float2 pcR = pc + float2(pixelSize, 0);
	float2 pcD = pc + float2(0, pixelSize);
	
	float3 c  = gi_BackBuffer.SampleLevel(g_sam_1, texcC, 0).rgb;
	float3 cR = gi_BackBuffer.SampleLevel(g_sam_1, pcR / size, 0).rgb;
	float3 cD = gi_BackBuffer.SampleLevel(g_sam_1, pcD / size, 0).rgb;
	
	// 경계 근처만 블렌딩: f가 1에 가까우면 오른쪽/아래로 넘어가는 느낌
	float edge = 0.18f; // 0.1~0.25 정도 취향
	float wx = smoothstep(1.0f - edge, 1.0f, f.x);
	float wy = smoothstep(1.0f - edge, 1.0f, f.y);
	
	float3 cx = lerp(c,  cR, wx);
	float3 cy = lerp(c,  cD, wy);
	float3 outc = (cx + cy) * 0.5f;
	
	return float4(outc, 1);
}

#endif // _PIXEL