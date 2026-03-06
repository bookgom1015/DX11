#ifndef _FLIPBOOK
#define _FLIPBOOK

#include "value.fx"

#define AtlasTex        g_tex_0

#define LeftTopUV       g_vec2_0
#define SliceUV         g_vec2_1
//#define BackgroundUV    g_vec2_2
#define BackgroundUV    g_vec2_1
#define OffsetUV        g_vec2_3

#define Albedo          g_vec4_1

struct VS_IN {
    float3 vPos : POSITION; // Sementic : Layout 에서 설명한 이름       
    float2 vUV : TEXCOORD;
};

struct VS_OUT {
    float4 vPosition : SV_Position; // 래스터라이져로 보낼때, NDC 좌표
    float2 vUV : TEXCOORD;
    float3 vWorldPos : POSITION;    
};

VS_OUT VS_Flipbook(VS_IN _input) {
    VS_OUT output = (VS_OUT)0;
             
    float4 vWorld = mul(float4(_input.vPos, 1.f), g_matWorld);
    float4 vView = mul(vWorld, g_matView);
    float4 vProj = mul(vView, g_matProj);
     
    output.vPosition = vProj;
    output.vWorldPos = vWorld;
    output.vUV = _input.vUV;
    
    return output;
}

// 0~1 입력 -> 밴드 단위 랜덤값
float Hash11(float p) {
    // cheap hash
    p = frac(p * 0.1031);
    p *= p + 33.33;
    p *= p + p;
    return frac(p);
}

float RandSigned(float p) {
    return Hash11(p) * 2.0f - 1.0f;
}

// 0..1 y -> 밴드 인덱스
float BandIndex(float y, float bandCount) {
    y = saturate(y);
    float b = floor(y * bandCount);
    return min(b, bandCount - 1.0);
}

// 밴드 경계가 딱 끊기는 걸 완화(선택)
float BandEdgeMask(float y, float bandCount) {
    float inBand = frac(saturate(y) * bandCount);
    return smoothstep(0.0, 0.12, inBand) * (1.0 - smoothstep(0.88, 1.0, inBand));
}

float4 MotionBlur2D(
        float2 uv,
        float2 motionDir,     // ex) float2(1,0) or float2(0,1) or normalize(vel)
        float strengthUV,     // blur length in UV
        int   samples,
        float bandCount,
        float seed,
        float time) {
    // 방향 정규화
    float len = max(length(motionDir), 1e-5);
    float2 dir = motionDir / len;

    // 밴드 기반 “미세 찢김” (원하면 끄면 됨)
    float b = BandIndex(uv.y, bandCount);
    float edge = BandEdgeMask(uv.y, bandCount);

    // 시간에 따라 밴드 랜덤이 바뀌게 (너무 흔들리면 time factor 줄여)
    float tstep = floor(time * 15.0);
    float bandRand = RandSigned(b + tstep * 17.0 + seed);

    // 밴드별로 블러 길이를 조금 다르게 해서 “속도감 + 글리치” 느낌
    float bandScale = 1.0 + bandRand * 0.25 * edge;

    float blurLen = strengthUV * bandScale;

    // 누적 샘플
    float4 sum = 0;
    float  wsum = 0;

    // 중앙(현재 프레임)을 포함해서 뒤로만 흐르는 블러(전형적 모션블러)
    // -i/(samples-1) 형태로 "뒤쪽" 샘플링
    [loop]
    for (int i = 0; i < samples; ++i) {
        float t = (samples <= 1) ? 0.0 : (float)i / (samples - 1); // 0..1
        float2 uvi = uv - dir * (t * blurLen);
        uvi = clamp(uvi, LeftTopUV, LeftTopUV + SliceUV);
        
        // 가중치: 가까운(현재) 쪽이 더 세게
        float w = 1.0 - t;              // 선형
        // 더 “샤프”하게 하고 싶으면: w = (1.0 - t) * (1.0 - t);

        float4 samp = AtlasTex.Sample(g_sam_1, uvi);
        if (samp.a < 1e-4f) continue;
        sum += samp * w;
        wsum += w;
    }

    return sum / max(wsum, 1e-5);
}

// 입력된 텍스쳐를 사용해서 픽셀쉐이더의 출력 색상으로 지정한다.
float4 PS_Flipbook(VS_OUT _input) : SV_Target {
    float2 vSpriteUV = _input.vUV * SliceUV + LeftTopUV;
    float4 vColor = (float4)0;
    
    // Atlas 텍스쳐가 바인딩이 되었으면
    if (g_btex_0) {
        float2 LeftTop = (LeftTopUV + SliceUV * 0.5f - BackgroundUV * 0.5f);
        float2 SampleUV = LeftTop + BackgroundUV * _input.vUV - OffsetUV;        
        
        if (LeftTopUV.x <= SampleUV.x && SampleUV.x <= LeftTopUV.x + SliceUV.x
           && LeftTopUV.y <= SampleUV.y && SampleUV.y <= LeftTopUV.y + SliceUV.y)
            vColor = AtlasTex.Sample(g_sam_1, SampleUV);
        else
            discard;
        
        if (vColor.a == 0.f) discard;        
    }  
    
    const float4 albedo = Albedo;
    vColor =  albedo.a * albedo + vColor;    
    
    // 광원 적용        
    float3 LightColor = CalcLight2D(Light2DCount, _input.vWorldPos);
    vColor.rgb *= LightColor;
    
    return vColor;
}

#endif