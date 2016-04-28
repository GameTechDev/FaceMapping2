//--------------------------------------------------------------------------------------
// Copyright 2012 Intel Corporation
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
    float3 Pos     : POSITION;
    float2 Uv      : TEXCOORD0;
};
struct PS_INPUT
{
    float4 Pos     : SV_POSITION;
    float2 Uv      : TEXCOORD0;
};

cbuffer cbGUIConstants
{
    row_major float4x4 wvpMatrix;
    float4 position;
    float4 backgroundcolor;
    float4 foregroundcolor;
    float4 screenresolution;
    float4 mousePosition;
};

cbuffer cbVisualizeConstants
{
    float scale;
    float uvoffset;
}

texture1D Values;


// ********************************************************************************************************
SamplerState SAMPLER0 : register( s0 );

// ********************************************************************************************************
PS_INPUT VSMain( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;

    float4 pos = float4(input.Pos.xy * position.zw, input.Pos.z, 1.0);
    pos.y *= -1;
    pos.xy -= screenresolution.xy/2.0;

    output.Pos = mul(pos, wvpMatrix);

    output.Uv   = input.Uv;
    
    return output;
}
// ********************************************************************************************************
float4 PSMain( PS_INPUT input ) : SV_Target
{
    if(mousePosition.x < input.Uv.x)
    {
        return foregroundcolor;
    }
    return backgroundcolor;
}
