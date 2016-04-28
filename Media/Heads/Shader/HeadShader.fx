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
struct VS_INPUT
{
    float3 Pos      : POSITION; // Projected position
    float3 Norm     : NORMAL;
    float2 Uv       : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos      : SV_POSITION;
    float3 Norm     : NORMAL;
    float3 Position : TEXCOORD0; // Object space position
    float4 LightUv  : TEXCOORD1;
    float2 Uv       : TEXCOORD2;
};

struct PS_INPUT_WIREFRAME
{
    float4 Pos      : SV_POSITION;
};

// ********************************************************************************************************
PS_INPUT VSMain( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos      = mul( WorldViewProjection, float4( input.Pos, 1.0f) );
    output.Position = mul(               World, float4( input.Pos, 1.0f)).xyz;
    // TODO: transform the light into object space instead of the normal into world space
    output.Norm     = mul( (float3x3)World, input.Norm );
    output.LightUv  = mul( LightWorldViewProjection, float4( input.Pos, 1.0f) );
    // output.Uv       = input.Uv;
#if 0 // Joe
    output.Uv       = input.Pos.xz * float2(3.67,-3.7);
    output.Uv       = output.Uv + float2(0.25, 0.42);
#endif
    // Aubrie
    output.Uv       = input.Pos.xz * float2(3.2,-3.2);
    output.Uv       = output.Uv + float2(0.3, 0.49);
    return output;
}

// ********************************************************************************************************
float4 PSMain( PS_INPUT input ) : SV_Target
{
    float3 lightUv          = input.LightUv.xyz; // Ortho light = No need for the divide.
    float2 uv               = lightUv.xy * 0.5f + 0.5f;
    float2 uvInvertY        = float2(uv.x, 1.0f-uv.y);

    float3 eyeDirection     = normalize(input.Position - InverseView._m30_m31_m32);
    float3 normal           = normalize(input.Norm);
    float  nDotL            = dot( normal, -normalize(LightDirection.xyz) );

//    float3 diffuseColor      = TEXTURE0.Sample( SAMPLER0, input.Uv );
    float3 diffuseColor      = float3( 0.5, 0.5, 0.5 );
    float3 diffuse           = diffuseColor * saturate(nDotL);
    float3 ambient           = AmbientColor * diffuseColor * saturate(-nDotL);

    float  shadowAmount      = _Shadow.SampleCmp( SAMPLER1, uvInvertY, lightUv.z );
    float3 finalShadowAmount = max( shadowAmount.xxx, AmbientColor );

    float3 result            = finalShadowAmount*diffuse + ambient;
    // float3 result            = diffuse + ambient;
    // float3 result            = diffuseColor;
    return float4( result, 1.0f);
}


// ********************************************************************************************************
PS_INPUT_WIREFRAME VSWireFrame( VS_INPUT input )
{
    PS_INPUT_WIREFRAME output = (PS_INPUT_WIREFRAME)0;

    float4 outputPos = mul( WorldViewProjection, float4( input.Pos, 1.0f) ); 
    outputPos.w *= 0.9999999f; // for ortho
    // outputPos.w *= 0.99f;     // for perspective
    output.Pos = outputPos;
    return output;
}

// ********************************************************************************************************
float4 PSWireFrame( PS_INPUT_WIREFRAME input ) : SV_Target
{
    return float4( 0.18, 0.18, 0.18, 1);
}
