#ifndef _TONE
#define _TONE

#include "value.fx"

struct VS_IN {
    float3 vPos     : POSITION; // Sementic : Layout 에서 설명한 이름       
    float2 vUV      : TEXCOORD;    
    float4 vColor   : COLOR;
};

struct VS_OUT {
    float3 PosW : POSITION0; // 래스터라이져로 보낼때, NDC 좌표
    float2 TexC : TEXCOORD0;
};

struct GS_OUT {
    float4 PosH     : SV_POSITION;
    float2 TexC     : TEXCOORD0;
};

VS_OUT VS_Shadow(in VS_IN vin) {
    VS_OUT output = (VS_OUT)0;                               
    
    output.PosW = mul(float4(vin.vPos, 1.f), g_matWorld).xyz;
    output.TexC = vin.vUV;
    
    return output;                                         
}

GS_OUT MakeOut(in float3 posW, in float2 uv) {
    GS_OUT gout = (GS_OUT)0;
    
    float4 posV = mul(float4(posW, 1.f), g_matView);    
    gout.PosH = mul(posV, g_matProj);
    gout.TexC = uv;
    
    return gout;
}

// TriangleStream에 "독립 삼각형"을 찍고 싶으면 3번 Append 후 RestartStrip
void EmitTri(inout TriangleStream<GS_OUT> stream,
                    in float3 aW, in float3 bW, in float3 cW,
                    in float2 aUV, in float2 bUV, in float2 cUV) {    
    stream.Append(MakeOut(aW, aUV));
    stream.Append(MakeOut(bW, bUV));
    stream.Append(MakeOut(cW, cUV));
    stream.RestartStrip();
}

void ExpandToZAxis(in triangle VS_OUT gin[3], inout TriangleStream<GS_OUT> stream) {
    // 입력 삼각형의 XY 범위를 구해서 "축 정렬" 박스를 만든다.
    float3 p0 = gin[0].PosW;
    float3 p1 = gin[1].PosW;
    float3 p2 = gin[2].PosW;

    float minX = min(p0.x, min(p1.x, p2.x));
    float maxX = max(p0.x, max(p1.x, p2.x));
    float minY = min(p0.y, min(p1.y, p2.y));
    float maxY = max(p0.y, max(p1.y, p2.y));

    // 이 삼각형이 놓인 "기준 Z" (XY 평면이면 거의 동일하겠지)
    float zBase = (p0.z + p1.z + p2.z) * (1.0f / 3.0f);
    float z0 = zBase - 10000.f;
    float z1 = zBase + 10000.f;

    // 8 corners (AABB box)
    // bottom(z0)
    float3 v000 = float3(minX, minY, z0);
    float3 v100 = float3(maxX, minY, z0);
    float3 v110 = float3(maxX, maxY, z0);
    float3 v010 = float3(minX, maxY, z0);
    // top(z1)
    float3 v001 = float3(minX, minY, z1);
    float3 v101 = float3(maxX, minY, z1);
    float3 v111 = float3(maxX, maxY, z1);
    float3 v011 = float3(minX, maxY, z1);

    // UV는 박스용으로 대충 매핑(필요 없으면 전부 (0,0) 넣거나 UV 제거)
    float2 uv00 = float2(0, 0);
    float2 uv10 = float2(1, 0);
    float2 uv11 = float2(1, 1);
    float2 uv01 = float2(0, 1);

    // 6 faces * 2 triangles = 12 triangles

    // +Z (top)
    EmitTri(stream, v001, v101, v111,  uv00, uv10, uv11);
    EmitTri(stream, v001, v111, v011,  uv00, uv11, uv01);

    // -Z (bottom) (winding 주의: 필요하면 순서 뒤집기)
    EmitTri(stream, v000, v110, v100,  uv00, uv11, uv10);
    EmitTri(stream, v000, v010, v110,  uv00, uv01, uv11);

    // +X
    EmitTri(stream, v100, v110, v111,  uv00, uv10, uv11);
    EmitTri(stream, v100, v111, v101,  uv00, uv11, uv01);

    // -X
    EmitTri(stream, v000, v011, v010,  uv00, uv11, uv10);
    EmitTri(stream, v000, v001, v011,  uv00, uv01, uv11);

    // +Y
    EmitTri(stream, v010, v011, v111,  uv00, uv10, uv11);
    EmitTri(stream, v010, v111, v110,  uv00, uv11, uv01);

    // -Y
    EmitTri(stream, v000, v101, v001,  uv00, uv11, uv10);
    EmitTri(stream, v000, v100, v101,  uv00, uv01, uv11);
}

[maxvertexcount(36)] // 12 triangles * 3 verts
void GS_Shadow(in triangle VS_OUT gin[3], inout TriangleStream<GS_OUT> stream) {
    ExpandToZAxis(gin, stream);
}

void PS_Shadow(in GS_OUT pin) {}

#endif // _TONE