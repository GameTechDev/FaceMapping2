//--------------------------------------------------------------------------------------
// Copyright 2014 Intel Corporation
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
    Texture2D  TEXTURE0;
    Texture2D  _Shadow;
    Texture2D  LightPropagation;

    SamplerState           SAMPLER0 : register(s0);
    SamplerComparisonState SAMPLER1 : register(s1);

// ********************************************************************************************************
cbuffer cbPerModelValues
{
    float4x4   World : WORLD;
    float4x4   WorldViewProjection : WORLDVIEWPROJECTION;
    float4x4   InverseWorld : INVERSEWORLD;
    float4     EyePosition;
    float4x4   LightWorldViewProjection;
    float4x4   ViewProjection;
    float4     BoundingBoxCenterWorldSpace;
    float4     BoundingBoxHalfWorldSpace;
    float4     BoundingBoxCenterObjectSpace;
    float4     BoundingBoxHalfObjectSpace;
    float4x4   WorldToVolume;
};
// ********************************************************************************************************
cbuffer cbPerFrameValues
{
    float4x4 View;
    float4x4 Projection;
    float3   AmbientColor;
    float3   LightColor;
    float3   LightDirection;
    float3   TotalTimeInSeconds;
    float4x4 InverseView;
    // float4x4 ViewProjection;
};

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
    //float3 Position : TEXCOORD0; // Object space position
    //float4 LightUv  : TEXCOORD1;
};

// ********************************************************************************************************
PS_INPUT_NO_TEX VSMainNoTexture( VS_INPUT_NO_TEX input )
{
    PS_INPUT_NO_TEX output = (PS_INPUT_NO_TEX)0;
    output.Pos      = mul( WorldViewProjection, float4( input.Pos, 1.0f) );
    //output.Position = mul(               World, float4( input.Pos, 1.0f)).xyz;
    // TODO: transform the light into object space instead of the normal into world space
    output.Norm = mul( (float3x3)World, input.Norm );
    //output.LightUv   = mul( LightWorldViewProjection, float4( input.Pos, 1.0f) );

    return output;
}

// ********************************************************************************************************
float4 PSMainNoTexture( PS_INPUT_NO_TEX input ) : SV_Target
{
    return float4( 0.0f,0.0f,1.0f, 1.0f);
}
