#ifndef _TEST
#define _TEST

#include "value.fx"


struct VS_IN
{
    float3 vPos     : POSITION; // Sementic : Layout 에서 설명한 이름       
    float2 vUV      : TEXCOORD;    
    float4 vColor   : COLOR;
};

struct VS_OUT
{
    float4 vPosition    : SV_Position;  // 래스터라이져로 보낼때, NDC 좌표
    float2 vUV          : TEXCOORD;
    float4 vColor       : COLOR;
};

VS_OUT VS_Std2D(VS_IN _input)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    float4 vWorld = mul(float4(_input.vPos, 1.f), g_matWorld);
    float4 vView = mul(vWorld, g_matView);
    float4 vProj = mul(vView, g_matProj);
        
    output.vPosition    = vProj;
    output.vUV          = _input.vUV;
    output.vColor       = _input.vColor;
    
    return output;
}

// 입력된 텍스쳐를 사용해서 픽셀쉐이더의 출력 색상으로 지정한다.
float4 PS_Std2D(VS_OUT _input) : SV_Target
{      
    // 입력 UV 는 정점에사 반환한 값을 보간받아서 픽셀쉐이더에 입력됨    
    float4 vColor = g_tex_0.Sample(g_sam_1, _input.vUV);
    
    if (vColor.a == 0.f || (vColor.r > 0.99f && vColor.b > 0.99f && vColor.g == 0.f))
    {
        discard;
    }
    
    if(g_int_0 == 1)
    {
        vColor.r *= 2.f;
    }
    
    return vColor;
}




#endif