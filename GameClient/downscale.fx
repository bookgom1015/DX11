#ifndef _DOWNSCALE
#define _DOWNSCALE

#include "value.fx"

#ifndef KERNEL_RADIUS
#define KERNEL_RADIUS 1
#endif

static const int KERNEL_SIZE = (KERNEL_RADIUS * 2);

Texture2D<float4>   gi_InputMap  : register(t0);
RWTexture2D<float4> go_OutputMap : register(u0);

[numthreads(8, 8, 1)]
void CS_DownScale(in uint2 DTid : SV_DispatchThreadID) {
    const uint2 DstIndex = DTid;    
    if (any(DstIndex >= gDstTexDim)) return;
    
    const uint2 SrcCenter = DstIndex * 2;
    
    const int2 BaseIndex = int2(SrcCenter) - int2(2, 2);
    
    float4 valueSum = 0.f;
    float weightSum = 0.f;
    
    [unroll]
    for (int y = 0; y < KERNEL_SIZE; ++y) {
        const int SrcY = clamp(BaseIndex.y + y, 0, int(gSrcTexDim.y) - 1);
        
        [unroll]
        for(int x = 0; x < KERNEL_SIZE; ++x) {
            const int SrcX = clamp(BaseIndex.x + x, 0, int(gSrcTexDim.x) - 1);
            
            const float4 Color = gi_InputMap.Load(int3(SrcX, SrcY, 0));
            valueSum += Color;
            weightSum += 1.f;
        }
    }
    
    go_OutputMap[DTid] = valueSum / weightSum;
}

#endif // _DOWNSCALE