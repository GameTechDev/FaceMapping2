//--------------------------------------------------------------------------------------
// Copyright 2013 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
//--------------------------------------------------------------------------------------

// ********************************************************************************************************
// -------------------------------------
struct VS_INPUT
{
    float3 Position : POSITION; // Projected position
    float3 Normal   : NORMAL;
    float2 UV0      : TEXCOORD0;
    float2 UV1      : TEXCOORD1;
//    float3 Tangent  : TANGENT;
//    float3 Binormal : BINORMAL;
};

// -------------------------------------
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 Normal   : NORMAL;
    //float3 Tangent  : TANGENT;
    //float3 Binormal : BINORMAL;
    float2 UV0      : TEXCOORD0;
    float2 UV1      : TEXCOORD1;
    float3 WorldPosition : TEXCOORD2; // Object space position 
    float3 Reflection : TEXCOORD3;
    float4 LightUV  : LIGHTSPACEPOSITION;
};

// ********************************************************************************************************
    Texture2D    Texture0 : register( t0 );
    SamplerState SAMPLER0 : register( s0 );
    Texture2D    _Shadow  : register( t1 );
    SamplerComparisonState SAMPLER1 : register( s1 );
// ********************************************************************************************************
// -------------------------------------
cbuffer cbPerModelValues
{
    row_major float4x4 World : WORLD;
    row_major float4x4 NormalMatrix : WORLD;
    row_major float4x4 WorldViewProjection : WORLDVIEWPROJECTION;
    row_major float4x4 InverseWorld : WORLDINVERSE;
    row_major float4x4 LightWorldViewProjection;
              float4   BoundingBoxCenterWorldSpace  < string UIWidget="None"; >;
              float4   BoundingBoxHalfWorldSpace    < string UIWidget="None"; >;
              float4   BoundingBoxCenterObjectSpace < string UIWidget="None"; >;
              float4   BoundingBoxHalfObjectSpace   < string UIWidget="None"; >;
};

// -------------------------------------
cbuffer cbPerFrameValues
{
    row_major float4x4 View;
    row_major float4x4  InverseView : ViewInverse	< string UIWidget="None"; >;
    row_major float4x4 Projection;
    row_major float4x4  ViewProjection;
              float4    AmbientColor < string UIWidget="None"; > = .20;
              float4    LightColor < string UIWidget="None"; >   = 1.0f;
              float4    LightDirection  : Direction < string UIName = "Light Direction";  string Object = "TargetLight"; string Space = "World"; int Ref_ID=0; > = {0,0,-1, 0};
              float4    EyePosition;
              float4    TotalTimeInSeconds < string UIWidget="None"; >;
};

// ********************************************************************************************************
PS_INPUT VSMain( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;

    output.Position      = mul( float4( input.Position, 1.0f), WorldViewProjection );
    output.WorldPosition = mul( float4( input.Position, 1.0f), World ).xyz;

    // TODO: transform the light into object space instead of the normal into world space
    output.Normal   = mul( input.Normal, (float3x3)World );
#ifdef USE_NORMALMAP
    output.Tangent  = mul( input.Tangent, (float3x3)World );
    output.Binormal = mul( input.Binormal, (float3x3)World );
#endif
    output.UV0 = input.UV0;
#ifdef UV_LAYER_1
    output.UV1 = input.UV1;
#endif
    output.LightUV = mul( float4( input.Position, 1.0f), LightWorldViewProjection );

    return output;
}

// ********************************************************************************************************
float4 PSMain( PS_INPUT input ) : SV_Target
{
    float3  lightUv = input.LightUV.xyz / input.LightUV.w;
    lightUv.xy = lightUv.xy * 0.5f + 0.5f; // TODO: Move scale and offset to matrix.
    lightUv.y  = 1.0f - lightUv.y;
    float   shadowAmount = _Shadow.SampleCmp( SAMPLER1, lightUv, lightUv.z ).r;
    float3 normal         = normalize(input.Normal);
    float  nDotL          = saturate( dot( normal, -LightDirection ) );
    float3 eyeDirection   = normalize(input.Position - InverseView._m30_m31_m32);
    float3 reflection     = reflect( eyeDirection, normal );
    float  rDotL          = saturate(dot( reflection, -LightDirection ));
    float3 specular       = pow(rDotL, 16.0f);
    specular              = min( shadowAmount, specular );
    float4 diffuseTexture = Texture0.Sample( SAMPLER0, input.UV0 );
    float ambient = 0.05;
    float3 result = (min(shadowAmount, nDotL)+ambient) * diffuseTexture + shadowAmount*specular;
    return float4( result, 1.0f );
}

// ********************************************************************************************************
struct VS_INPUT_NO_TEX
{
    float3 Pos      : POSITION; // Projected position
    float3 Norm     : NORMAL;
};
struct PS_INPUT_NO_TEX
{
    float4 Pos      : SV_POSITION;
    float3 Norm     : NORMAL;
    float4 LightUv  : TEXCOORD1;
    float3 Position : TEXCOORD0; // Object space position 
};
// ********************************************************************************************************
PS_INPUT_NO_TEX VSMainNoTexture( VS_INPUT_NO_TEX input )
{
    PS_INPUT_NO_TEX output = (PS_INPUT_NO_TEX)0;
    output.Pos      = mul( float4( input.Pos, 1.0f), WorldViewProjection );
    output.Position = mul( float4( input.Pos, 1.0f), World ).xyz;
    // TODO: transform the light into object space instead of the normal into world space
    output.Norm = mul( input.Norm, (float3x3)World );
    output.LightUv   = mul( float4( input.Pos, 1.0f), LightWorldViewProjection );
    return output;
}
// ********************************************************************************************************
float4 PSMainNoTexture( PS_INPUT_NO_TEX input ) : SV_Target
{
    float3 lightUv = input.LightUv.xyz / input.LightUv.w;
    float2 uv = lightUv.xy * 0.5f + 0.5f;
    float2 uvInvertY = float2(uv.x, 1.0f-uv.y);
    float shadowAmount = _Shadow.SampleCmp( SAMPLER1, uvInvertY, lightUv.z );
    float3 eyeDirection = normalize(input.Position - InverseView._m30_m31_m32);
    float3 normal       = normalize(input.Norm);
    float  nDotL = saturate( dot( normal, -normalize(LightDirection.xyz) ) );
    nDotL = shadowAmount * nDotL;
    float3 reflection   = reflect( eyeDirection, normal );
    float  rDotL        = saturate(dot( reflection, -LightDirection.xyz ));
    float  specular     = 0.2f * pow( rDotL, 4.0f );
    specular = min( shadowAmount, specular );
    return float4( (nDotL + specular).xxx, 1.0f);
}



