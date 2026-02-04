#ifndef _VALUE
#define _VALUE

SamplerState g_sam_0 : register(s0);
SamplerState g_sam_1 : register(s1);

cbuffer TRANSFORM : register(b0)
{
    row_major matrix g_matWorld; // Local(Model) -> World Space
    row_major matrix g_matView; // World -> Camea(View) Space
    row_major matrix g_matProj; // Camera(View) -> Projection Space
}

cbuffer MATERIAL : register(b1)
{
    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;
    
    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;
    
    float2 g_vec2_0;
    float2 g_vec2_1;
    float2 g_vec2_2;
    float2 g_vec2_3;
    
    float4 g_vec4_0;
    float4 g_vec4_1;
    float4 g_vec4_2;
    float4 g_vec4_3;
    
    row_major Matrix g_mat_0;
    row_major Matrix g_mat_1;
}

Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);
Texture2D g_tex_5 : register(t5);

#endif