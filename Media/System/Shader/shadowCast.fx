//--------------------------------------------------------------------------------------
// Copyright 2011 Intel Corporation
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
struct VS_INPUT
{
    float3 Pos      : POSITION; // Projected position
};
struct PS_INPUT
{
    float4 Pos      : SV_POSITION;
};
struct VS_INPUT_SKINNED
{
    float3 Position : POSITION; // Projected position
    float3 Normal   : NORMAL;
    float3 Tangent  : TANGENT;
    float3 Binormal : BINORMAL;
    float2 UV0      : TEXCOORD0;
	float4 blendWeight: BLEND_WEIGHT;
	uint4  blendIndex: BLEND_INDEX;
};

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

cbuffer cbSkinningValues
{
    row_major float4x4 SkinMatrix[255];
    row_major float4x4 SkinNormalMatrix[255];
};

// -------------------------------------
cbuffer cbPerFrameValues
{
    row_major float4x4  View;
    row_major float4x4  InverseView : ViewInverse	< string UIWidget="None"; >;
    row_major float4x4  Projection;
    row_major float4x4  ViewProjection;
              float4    AmbientColor < string UIWidget="None"; > = .20;
              float4    LightColor < string UIWidget="None"; >   = 1.0f;
              float4    LightDirection  : Direction < string UIName = "Light Direction";  string Object = "TargetLight"; string Space = "World"; int Ref_ID=0; > = {0,0,-1, 0};
              float4    EyePosition;
              float4    TotalTimeInSeconds < string UIWidget="None"; >;
};
// ********************************************************************************************************
//cbuffer cbPerModelValues
//{
//    row_major float4x4 World : WORLD;
//    row_major float4x4 WorldViewProjection : WORLDVIEWPROJECTION;
//    row_major float4x4 InverseWorld : INVERSEWORLD;
//              float4   LightDirection;
//              float4   EyePosition;
//    row_major float4x4 LightWorldViewProjection;
//};
//
//// ********************************************************************************************************
//// TODO: Note: nothing sets these values yet
//cbuffer cbPerFrameValues
//{
//    row_major float4x4  View;
//    row_major float4x4  Projection;
//};

// ********************************************************************************************************
PS_INPUT VSMain( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( float4( input.Pos, 1.0f), WorldViewProjection );
    output.Pos.z -= 0.0001 * output.Pos.w;
    return output;
}

// ********************************************************************************************************
float4 PSMain( PS_INPUT input ) : SV_Target
{
    // TODO: Don't have pixel shader.
    return float4(1,0,0,1);
}
// ********************************************************************************************************
PS_INPUT VSMainSkinned( VS_INPUT_SKINNED input )
{
    PS_INPUT output = (PS_INPUT)0;

	float4 pos = float4( input.Position, 1.0f);
	float4 animPos = 0;

	
	
	animPos += mul(pos, SkinMatrix[input.blendIndex[0]])*input.blendWeight[0];
	animPos += mul(pos, SkinMatrix[input.blendIndex[1]])*input.blendWeight[1];
	animPos += mul(pos, SkinMatrix[input.blendIndex[2]])*input.blendWeight[2];
	animPos += mul(pos, SkinMatrix[input.blendIndex[3]])*input.blendWeight[3];
	

	output.Pos      = mul( animPos, WorldViewProjection );
	output.Pos.z -= 0.0001 * output.Pos.w;

    return output;
}