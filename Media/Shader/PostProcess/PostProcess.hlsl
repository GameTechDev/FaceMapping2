
//--------------------------------------------------------------------------------------
Texture2D    TEXTURE0 : register( t0 );
Texture2D    TEXTURE1 : register( t1 );
Texture2D    TEXTURE2 : register( t2 );
SamplerState SAMPLER0 : register( s0 );
SamplerState SAMPLER1 : register( s1 );

//--------------------------------------------------------------------------------------
struct VS_OUTPUT10
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
VS_OUTPUT10 VSMain( 
    float3 inPosition : POSITION,
    float2 inUv       : TEXCOORD0
 )
{
    VS_OUTPUT10 Out;

    Out.Position = float4(inPosition, 1.0f);
    Out.Position.z = 0.5;
    Out.Position.w = 1.0;
    Out.Uv = inUv;

    return Out;
}

//--------------------------------------------------------------------------------------
float4 DownSample4x4PS( VS_OUTPUT10 In ) : SV_Target
{
    // Compute the average of an 4x4 pixel block by sampling 2x2 bilinear-filtered samples.
    float4 color = 
    (
          TEXTURE0.Sample( SAMPLER0, In.Uv, int2(0, 0) )
       +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(2, 0) )
       +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(0,-2) )
       +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(2,-2) )
    )
    * (1.0f/4.0f);
    return color;
}

//--------------------------------------------------------------------------------------
float4 DownSample4x4AlphaPS( VS_OUTPUT10 In ) : SV_Target
{
    // Larger=faster adaptation.  Smaller=slower.
    // 1.0=instantaneous.  0.0=none.
    // TODO: adjust for frame time.  Now works per-frame,
    // so faster frame rate = faster adapting
    // Also, pass in from program instead of hard-coding.
    float gExposureAdaptSpeed=0.01;

    // Compute the average of an 4x4 pixel block by sampling 2x2 bilinear-filtered samples.
    float4 color = 
    (
          TEXTURE0.Sample( SAMPLER0, In.Uv, int2(0, 0) )
       +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(2, 0) )
       +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(0,-2) )
       +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(2,-2) )
    )
    * (1.0f/4.0f);

    // Blend in just a little bit of the result.  We use this for
    // the tone-mapping's adaptive-exposure.
    return float4(color.xyz, gExposureAdaptSpeed);
}

//--------------------------------------------------------------------------------------
float4 BlurHorizontalPS( VS_OUTPUT10 In ) : SV_Target
{
    float gWeights[5] = {    
        0.28525234,
        0.221024189,
        0.102818575,
        0.028716039,
        0.004815026
    };

    // Compute a 17-tap gaussian blur using 9 bilinear samples.
    float4 color = 
          gWeights[0] *  TEXTURE0.Sample( SAMPLER1, In.Uv, int2( 0, 0) ) // use point-sampling for center sample
        + gWeights[1] * (TEXTURE0.Sample( SAMPLER0, In.Uv, int2(-1, 0) )
                      +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(+1, 0) ))
        + gWeights[2] * (TEXTURE0.Sample( SAMPLER0, In.Uv, int2(-2, 0) )
                      +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(+2, 0) ))
        + gWeights[3] * (TEXTURE0.Sample( SAMPLER0, In.Uv, int2(-3, 0) )
                      +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(+3, 0) ))
        + gWeights[4] * (TEXTURE0.Sample( SAMPLER0, In.Uv, int2(-4, 0) )
                      +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(+4, 0) ));
    return color;
}


//--------------------------------------------------------------------------------------
float4 BlurVerticalPS( VS_OUTPUT10 In ) : SV_Target
{
    float gWeights[5] = {    
        0.28525234,
        0.221024189,
        0.102818575,
        0.028716039,
        0.004815026
    };

    // Compute a 17-tap gaussian blur using 9 bilinear samples.
    float4 color = 
          gWeights[0] *  TEXTURE0.Sample( SAMPLER1, In.Uv, int2(0, 0) ) // use point-sampling for center sample
        + gWeights[1] * (TEXTURE0.Sample( SAMPLER0, In.Uv, int2(0,-1) )
                      +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(0,+1) ))
        + gWeights[2] * (TEXTURE0.Sample( SAMPLER0, In.Uv, int2(0,-2) )
                      +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(0,+2) ))
        + gWeights[3] * (TEXTURE0.Sample( SAMPLER0, In.Uv, int2(0,-3) )
                      +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(0,+3) ))
        + gWeights[4] * (TEXTURE0.Sample( SAMPLER0, In.Uv, int2(0,-4) )
                      +  TEXTURE0.Sample( SAMPLER0, In.Uv, int2(0,+4) ));
    return color;
}

//--------------------------------------------------------------------------------------
float4 DownSampleLogLumPS( VS_OUTPUT10 In ) : SV_Target
{
    float4 color = TEXTURE0.Sample( SAMPLER0, In.Uv );
    
    // Compute luminance
    const float3 rgbToLum = float3( 0.299, 0.587, 0.114 );
    float luminance = dot(color, rgbToLum);
    
    // Return the log(luminance+epsilon)
    float result = log(luminance + 0.001); // avoid log(0)
    return float4(result.xxx, 1);
}

//--------------------------------------------------------------------------------------
float4 CompositePS( VS_OUTPUT10 In ) : SV_Target
{
    // TODO: Move these to constant buffer.  Measure perf impact.  Consider also supporting hard-coded if good perf win.
    const float gBloomAmount  = 1.0f;
    const float gMiddleGrey   = 0.33f;
    const float LwhiteSquared = 0.7f;

    float3 originalColor = TEXTURE1.Sample( SAMPLER1, In.Uv );

    // Compute the average of an 8x8 pixel block by sampling 4x4 bilinear-filtered samples.
    float3 color = 
    (
        originalColor  
        + gBloomAmount * TEXTURE0.Sample( SAMPLER0, In.Uv )
    );
    
    float Lw = exp(TEXTURE2.Sample( SAMPLER0, float2(0,0) ) );

    const float3 rgbToLum = float3(.3,.59,.11);
    float luminance = dot(color, rgbToLum);
    float L = gMiddleGrey * luminance/Lw;
    float Ld = (L*(1+L/LwhiteSquared))/(1+L);
    
    color = saturate(color * Ld);
    
    return float4(color.xyz, 1);
}

