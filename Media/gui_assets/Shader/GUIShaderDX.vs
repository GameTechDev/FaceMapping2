//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
Texture2D GUITexture : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_INPUT
{
    float4 Pos	: POSITION;
    float2 Tex	: TEXCOORD;
    float4 Color: COLOR;    
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
    float4 Color: COLOR;
};

cbuffer cbGUIValues : register( b0 )
{    	
     row_major matrix  projectionMatrix;
     row_major matrix  modelMatrix;
};

PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;

    input.Pos.w = 1.0f;
	
	output.Pos = mul( mul( input.Pos, modelMatrix ), projectionMatrix );
 
    output.Tex = input.Tex;

    output.Color = input.Color;
    
    return output;
}
