#ifndef _TILE
#define _TILE

#include "value.fx"


struct SpriteInfo
{
    float2 LeftTop;
    float2 Slice;
};
StructuredBuffer<SpriteInfo> g_Buffer : register(t20);


#define AtlasTex    g_tex_0
#define ROW         g_int_0
#define COL         g_int_1
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
    float3 vWorld       : POSITION;
};

VS_OUT VS_Tile(VS_IN _input)
{
    VS_OUT output = (VS_OUT) 0.f;
             
    _input.vPos.xy += float2(0.5f, -0.5f);    
    
    float4 vWorld = mul(float4(_input.vPos, 1.f), g_matWorld);
    float4 vView = mul(vWorld, g_matView);
    float4 vProj = mul(vView, g_matProj);
     
    output.vPosition = vProj;
    output.vUV = _input.vUV * float2(COL, ROW);
    output.vWorld = vWorld;
    
    return output;
}

// 입력된 텍스쳐를 사용해서 픽셀쉐이더의 출력 색상으로 지정한다.
float4 PS_Tile(VS_OUT _input) : SV_Target
{
    int2 ColRow = int2(_input.vUV);
    int Idx = ColRow.y * COL + ColRow.x;
    
    float2 vSpriteUV = frac(_input.vUV) * g_Buffer[Idx].Slice + g_Buffer[Idx].LeftTop;
    float4 vColor = AtlasTex.Sample(g_sam_1, vSpriteUV);
        
    if (vColor.a == 0.f)
        discard;
    
    // 광원 적용
    float3 LightColor = CalcLight2DWithShadow(Light2DCount, _input.vWorld, ShadowTex);
    vColor.rgb *= LightColor;
    return vColor;
}




#endif