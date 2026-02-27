#ifndef _SPRITE
#define _SPRITE

#include "value.fx"

#define AtlasTex    g_tex_0
#define LeftTopUV   g_vec2_0
#define SliceUV     g_vec2_1
#define Emit        g_int_2
#define ShadowTex   g_tex_shadow

struct VS_IN
{
    float3 vPos : POSITION; // Sementic : Layout 에서 설명한 이름       
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position; // 래스터라이져로 보낼때, NDC 좌표
    float2 vUV : TEXCOORD;
    float3 vWorldPos : POSITIONT;
};

VS_OUT VS_Sprite(VS_IN _input)
{
    VS_OUT output = (VS_OUT) 0.f;
             
    float4 vWorld = mul(float4(_input.vPos, 1.f), g_matWorld);
    float4 vView = mul(vWorld, g_matView);
    float4 vProj = mul(vView, g_matProj);
     
    output.vPosition = vProj;
    output.vUV = _input.vUV;
    output.vWorldPos = vWorld;
    
    return output;
}

// 0~1 입력 -> 밴드 단위 랜덤값
float Hash11(float p)
{
    // cheap hash
    p = frac(p * 0.1031);
    p *= p + 33.33;
    p *= p + p;
    return frac(p);
}

// 밴드 인덱스 + seed로 -1~1 랜덤
float RandBandSigned(float bandIndex, float seed)
{
    float r = Hash11(bandIndex + seed);
    return r * 2.0f - 1.0f;
}

// vUV.y에 따라 밴드별로 x 오프셋 주는 글리치
float2 GlitchOffset_ByBands(float2 uv, float time, float bandCount, float seed, float amp, bool bHorz = true)
{
    // 1) 밴드 인덱스 (0..bandCount-1)
    float y;
    if (bHorz) y = saturate(uv.y);
    else y = saturate(uv.x);
    
    float bandF = floor(y * bandCount);
    float bandIndex = min(bandF, bandCount - 1.0);

    // 2) 밴드 내부 위치 (0..1) -> 경계 스무딩용
    float inBand = frac(y * bandCount);

    // 3) 밴드별 랜덤 오프셋 (-1..1)
    // time을 섞어주면 "시간에 따라 변하는 층 글리치"가 됨
    float tstep = floor(time * 20.0);               // 20Hz로 갱신 (원하면 조절)
    float r = RandBandSigned(bandIndex + tstep * 17.0, seed);

    // 4) y 위치별 강도 커브 (중간이 강하고 위/아래 약하게 등)
    // 원하는 느낌으로 커브 바꾸면 됨.
    float envelope = smoothstep(0.05, 0.35, y) * (1.0 - smoothstep(0.75, 0.98, y));

    // 5) 밴드 경계가 딱 끊기는 게 싫으면 부드럽게
    float edgeMask = smoothstep(0.0, 0.12, inBand) * (1.0 - smoothstep(0.88, 1.0, inBand));

    // 6) 최종 오프셋
    //float amp = 0.02; // 기본 강도 (uv 기준이니까 0.01~0.05 사이에서 튜닝)
    float offset = r * amp * envelope * edgeMask;
    
    if (bHorz) return float2(offset, 0.f);
    else return float2(0.f, offset);
}

// 입력된 텍스쳐를 사용해서 픽셀쉐이더의 출력 색상으로 지정한다.
float4 PS_Sprite(VS_OUT _input) : SV_Target
{    
    float time = g_float_0;
    
    float2 vSpriteUV = _input.vUV * SliceUV + LeftTopUV;
    
    float2 offset = (float2)0;
    if (g_int_0 == 1) offset = GlitchOffset_ByBands(_input.vUV , time, 6.0, 123.45, 0.02f);
    if (g_int_1 == 1) offset = GlitchOffset_ByBands(_input.vUV , time * 0.25f, 1024.0, 123.45, 0.025f, false);
        
    float2 finalUV = vSpriteUV + offset;
    finalUV = clamp(finalUV, LeftTopUV, LeftTopUV + SliceUV);
    
    float4 vColor = AtlasTex.Sample(g_sam_1, finalUV);
    
    if(vColor.a == 0.f) discard;
        
    const float4 albedo = g_vec4_1;
    
    const float3 relicFromColor = (float3)-0.8f;
    
    if (g_int_1 == 1) 
        vColor = float4((1.f - offset.y) * relicFromColor + offset.y + albedo.rgb, 0.f) + vColor;
    else 
        vColor = albedo.a * albedo + vColor;
    
    vColor = max(vColor, 0.f);
    
    // 광원 적용  
    if (!Emit) {
        float3 LightColor = CalcLight2DWithShadow(Light2DCount, _input.vWorldPos, ShadowTex);
        vColor.rgb *= LightColor;    
    }
    return vColor;
}





#endif