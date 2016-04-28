//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
#ifdef textured
Texture2D SpriteTexture : register( t0 );
SamplerState samLinear : register( s0 );
#endif
struct VS_INPUT
{
    float4 Pos	: POSITION;
    float2 Tex	: TEXCOORD;
//    float4 Color: COLOR;    
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
    float4 Color: COLOR;
};

cbuffer cbGUIConstants
{
    row_major float4x4 wvpMatrix;    
    float4 position;
    float4 backgroundcolor;
    float4 textColor;
    float4 screenresolution;
};

PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;

    float4 pos = float4( input.Pos.xyz, 1.0f);
    pos.xy -= screenresolution.xy/2.0;
    
    output.Pos = mul(pos, wvpMatrix);
    output.Tex = input.Tex;
    //output.Color = input.Color;
    
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

// alpha blended texture modulated by vertex color
// assumes L8 texture.
float4 PS( PS_INPUT input) : SV_Target
{
#ifdef textured
    return SpriteTexture.Sample( samLinear, input.Tex ).rrrr * textColor; 
#else
    return float4(textColor.rgb, 1.0);;
#endif
}


