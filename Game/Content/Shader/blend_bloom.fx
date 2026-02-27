#ifndef _BLEND_BLOOM
#define _BLEND_BLOOM

#include "value.fx"

Texture2D<float4>   gi_LowerScaleMap   : register(t0);
RWTexture2D<float4> gio_HigherScaleMap : register(u0);

[numthreads(8, 8, 1)]
void CS_BlendBloom(in uint2 DTid : SV_DispatchThreadID) {
    const float2 TexC = ((float2)DTid + 0.5f) * gInvTexDim;

    const float4 LowSample = gi_LowerScaleMap.SampleLevel(g_sam_0, TexC, 0);
    const float4 HighSample = gio_HigherScaleMap[DTid];
    
    gio_HigherScaleMap[DTid] = lerp(HighSample, LowSample, 0.5f);
}

#endif // _BLEND_BLOOM