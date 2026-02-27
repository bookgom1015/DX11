#ifndef _FUNC
#define _FUNC

#include "value.fx"

float CalcArtAttenuation(in float d, in float r) {
    float x = saturate(1 - d / r);  // 선형
    return x * x;                   // 제곱(원하면 x^3, x^4)
}

float3 ClosestPointOnSegment(float3 A, float3 B, float3 P) {
    float3 AB = B - A;
    float  ab2 = dot(AB, AB);              // |AB|^2

    // AB 길이가 0이면(Length=0) A가 답
    if (ab2 < 1e-8f) return A;

    float t = dot(P - A, AB) / ab2;        // 무한 직선에서의 파라미터
    t = saturate(t);                       // 선분으로 클램프 [0,1]

    return A + AB * t;
}

float3 ClosestPointOnPlane(float3 P, float3 Q, float3 N_unit) {
    float d = dot(P - Q, N_unit);   // 부호 있는 거리 (N 방향 + / -)
    return P - d * N_unit;          // 평면으로 “내려앉힘”
}

float PointLight(int _LightIdx, float3 _PixelPos) {
    // 광원에서 픽셀을 향하는 방향벡터(월드공간 기준)
    float3 LightToPixel = _PixelPos - g_Light2D[_LightIdx].WorldPos;
    
    // 2차원으로만 거리를 따질 것이기 때문에, z 성분 제거
    LightToPixel.z = 0.f;        
    
    // 벡터의 길이 측정(광원과 픽셀 사이의 거리)
    float Dist = length(LightToPixel);    
    
    // 물체와 광원의 거리가 멀어질수록 빛의 세기가 0 에 가까워진다.
    //DistRatio = saturate(1.f - Dist / g_Light2D[_LightIdx].Radius);        
    return CalcArtAttenuation(Dist, g_Light2D[_LightIdx].Radius);
}

float SpotLight(int _LightIdx, float3 _PixelPos) {
    float3 LightToPixel = _PixelPos - g_Light2D[_LightIdx].WorldPos;    
    LightToPixel.z = 0.f;
    
    float3 dir = normalize(LightToPixel);
    float3 lightDir = float3(1.f, 0.f, 0.f);
    
    float angle = g_Light2D[_LightIdx].Angle * 0.5f;
    float dp = max(dot(dir, g_Light2D[_LightIdx].LightDir), 0.f);
    float ac = acos(dp);
    dp = max((angle - ac) / angle, 0.f);
    
    float Dist = length(LightToPixel);
    return CalcArtAttenuation(Dist, g_Light2D[_LightIdx].Radius) * dp;
}

float LineLight(int _LightIdx, float3 _PixelPos) {
    float3 posW = g_Light2D[_LightIdx].WorldPos;
    float3 lightDir = g_Light2D[_LightIdx].LightDir;
    float lightLength = g_Light2D[_LightIdx].Length;
    
    float3 right = cross(float3(0.f, 0.f, -1.f), lightDir);
    float3 left = right * -1.f;
    
    float3 lposL = posW + left * lightLength * 0.5f;
    float3 lposR = posW + right * lightLength * 0.5f;
    
    float3 closestPoint = ClosestPointOnSegment(lposL, lposR, _PixelPos); 
    
    float3 toPixel = normalize(_PixelPos - closestPoint);
    float dp = max(dot(toPixel, lightDir), 0.f);
    if (dp > 1e4f) dp = 1.f;
    
    float Dist = length(_PixelPos - closestPoint);
    return CalcArtAttenuation(Dist, g_Light2D[_LightIdx].Radius) * dp;
}

float RectLight(int _LightIdx, float3 _PixelPos) {
    float3 posW = g_Light2D[_LightIdx].WorldPos;
    
    float3 lightDir = g_Light2D[_LightIdx].LightDir;
    float size = g_Light2D[_LightIdx].Length;
    
    float3 forward = float3(0.f, 0.f, 1.f);
    float3 up = cross(forward, lightDir);
    float3 right = lightDir;
    
    float3 maxP = posW + right * size * 0.5 + up * size * 0.5;
    float3 minP = posW - right * size * 0.5 - up * size * 0.5;
    
    float3 closestPoint = clamp(_PixelPos, minP, maxP);
                
    _PixelPos.z = 0.f;
    closestPoint.z = 0.f;
    
    float Dist = length(_PixelPos - closestPoint);
    return CalcArtAttenuation(Dist, g_Light2D[_LightIdx].Radius);
}

float CalcShadowPCF(
        in uint2 size, in float2 texc, in float d, 
        in Texture2DArray shadowTex, in int _LightIdx, in int offset) {    
    const int Radius = 3;
    const int Diameter = Radius * 2  + 1;    
    
    float2 ddxy = 1.f / size;
    
    float factor = 0.f;
    
    [loop]
    for (int dy = -Radius; dy <= Radius; ++dy) {
        [loop]
        for (int dx = -Radius; dx <= Radius; ++dx) {
            float2 neighbor = texc + float2(dx * ddxy.x, dy * ddxy.y);
    
            float depth = shadowTex.Sample(
                g_sam_1, float3(neighbor, g_Light2D[_LightIdx].BaseIndex + offset)).r;    
            if (depth < d) factor += 1.f;
        }
    }
    factor /= (Diameter * Diameter);
    
    return factor;
}

float CalcSpotShadowFactor(
        in Texture2DArray shadowTex, in int _LightIdx, in float3 _PixelPos) {
    float4 posV = mul(float4(_PixelPos, 1.f), g_Light2D[_LightIdx].Views[0]);
    float4 posH = mul(posV, g_Light2D[_LightIdx].Projs[0]);
    posH /= posH.w;
    
    float2 texc = posH.xy * 0.5f + 0.5f;
    texc.y = 1.f - texc.y;
    
    uint2 size;
    uint elements;
    shadowTex.GetDimensions(size.x, size.y, elements);    
    
    float factor = CalcShadowPCF(size, texc, posH.z, shadowTex, _LightIdx, 0);
    
    return 1.f - factor;
}

float CalcLineShadowFactor(
        in Texture2DArray shadowTex, in int _LightIdx, in float3 _PixelPos) {    
    uint2 size;
    uint elements;
    shadowTex.GetDimensions(size.x, size.y, elements);    
    
    float finalFactor = 0.f;
    [loop]
    for (int i = 0; i < g_Light2D[_LightIdx].Size; ++i) {
        float4 posV = mul(float4(_PixelPos, 1.f), g_Light2D[_LightIdx].Views[i]);
        float4 posH = mul(posV, g_Light2D[_LightIdx].Projs[i]);
        posH /= posH.w;
        
        float2 texc = posH.xy * 0.5f + 0.5f;
        texc.y = 1.f - texc.y;
        
        float factor = CalcShadowPCF(size, texc, posH.z, shadowTex, _LightIdx, i);    
        finalFactor += factor;
    }
    
    return 1.f - (finalFactor * (1.f / (float)g_Light2D[_LightIdx].Size));
}

float CalcPointShadowFactor(
        in Texture2DArray shadowTex, in int _LightIdx, in float3 _PixelPos) {    
    uint2 size;
    uint elements;
    shadowTex.GetDimensions(size.x, size.y, elements);    
        
    float finalFactor = 0.f;
    [loop]
    for (int i = 0; i < g_Light2D[_LightIdx].Size; ++i) {
        float4 posV = mul(float4(_PixelPos, 1.f), g_Light2D[_LightIdx].Views[i]);        
        float4 posH = mul(posV, g_Light2D[_LightIdx].Projs[i]);
        if (posH.z < 0.f) continue;
        
        posH /= posH.w;
        if (any(posH.xy > 1.f) || any(posH.xy < -1.f)) continue;                
        
        float2 texc = posH.xy * 0.5f + 0.5f;
        texc.y = 1.f - texc.y;
        
        float factor = CalcShadowPCF(size, texc, posH.z, shadowTex, _LightIdx, i);     
        finalFactor += factor;
    }
    
    return 1.f - (finalFactor * (1.f / (float)g_Light2D[_LightIdx].Size));
}

float3 CalcLight2D(int _LightCount, float3 _PixelPos) {
    float3 LightColor = (float3)0;
    
    // 광원과 물체의 거리에 따른 빛의 세기 비율
    float DistRatio = 1.f;
    
    for (int _LightIdx = 0; _LightIdx < _LightCount; ++_LightIdx) {
        if (g_Light2D[_LightIdx].Disabeld) continue;
        
        float3 color = g_Light2D[_LightIdx].Color * g_Light2D[_LightIdx].Intensity;
        // DirectionalLight
        if (0 == g_Light2D[_LightIdx].Type ) 
            color += g_Light2D[_LightIdx].Ambient;
        // Point
        else if (1 == g_Light2D[_LightIdx].Type) 
            DistRatio = PointLight(_LightIdx, _PixelPos);     
        // Spot
        else if (2 == g_Light2D[_LightIdx].Type) 
            DistRatio = SpotLight(_LightIdx, _PixelPos);
        // Line
        else if (3 == g_Light2D[_LightIdx].Type) 
            DistRatio = LineLight(_LightIdx, _PixelPos);        
        // Rect
        else if (4 == g_Light2D[_LightIdx].Type) {
            DistRatio = RectLight(_LightIdx, _PixelPos);
        }
        
        LightColor += color * DistRatio;
    }
    
    return LightColor;
}

float3 CalcLight2DWithShadow(
        int _LightCount, float3 _PixelPos, in Texture2DArray shadowTex) {
    float3 LightColor = (float3)0;
    
    // 광원과 물체의 거리에 따른 빛의 세기 비율
    float DistRatio = 1.f;
    
    for (int _LightIdx = 0; _LightIdx < _LightCount; ++_LightIdx) {
        if (g_Light2D[_LightIdx].Disabeld) continue;
        
        float3 color = g_Light2D[_LightIdx].Color * g_Light2D[_LightIdx].Intensity;
        // DirectionalLight
        if (0 == g_Light2D[_LightIdx].Type ) {
            color += g_Light2D[_LightIdx].Ambient;
        }
        // Point
        else if (1 == g_Light2D[_LightIdx].Type) {
            float factor = CalcPointShadowFactor(shadowTex, _LightIdx, _PixelPos);
            DistRatio = factor * PointLight(_LightIdx, _PixelPos);     
        }
        // Spot
        else if (2 == g_Light2D[_LightIdx].Type)  {
            float factor = CalcSpotShadowFactor(shadowTex, _LightIdx, _PixelPos);
            DistRatio = factor * SpotLight(_LightIdx, _PixelPos);   
        }
        // Line
        else if (3 == g_Light2D[_LightIdx].Type) {
            float factor = CalcLineShadowFactor(shadowTex, _LightIdx, _PixelPos);
            DistRatio = factor * LineLight(_LightIdx, _PixelPos);
        }
        // Rect
        else if (4 == g_Light2D[_LightIdx].Type) {
            DistRatio = RectLight(_LightIdx, _PixelPos);
        }
        
        LightColor += color * DistRatio;
    }
    
    return LightColor;
}

#endif


