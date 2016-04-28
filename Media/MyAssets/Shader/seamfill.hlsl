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

#define SAMPLE_CELLS 8

// -------------------------------------
struct VS_INPUT
{
    float3 Position : POSITION; // Projected position
};

// -------------------------------------
struct PS_INPUT
{
    float4 Position : SV_POSITION;
};

// -------------------------------------
SamplerState SAMPLER0 : register( s0 );
Texture2D TEXTURE0 : register( t0 );

// -------------------------------------
PS_INPUT VSMain(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Position = float4(input.Position, 1.0f);

	return output;
}

// -------------------------------------
float4 PSMain(PS_INPUT input) : SV_Target
{
	
	float4 col = TEXTURE0.Load(uint3(input.Position.x, input.Position.y, 0), 0);
	if (col.a < 0.02f)
	{
		float4 samp[SAMPLE_CELLS];
		samp[0] = TEXTURE0.Load(uint3(input.Position.x-1, input.Position.y, 0), 0);
		samp[1] = TEXTURE0.Load(uint3(input.Position.x+1, input.Position.y, 0), 0);
		samp[2] = TEXTURE0.Load(uint3(input.Position.x, input.Position.y-1, 0), 0);
		samp[3] = TEXTURE0.Load(uint3(input.Position.x, input.Position.y + 1, 0), 0);
#if SAMPLE_CELLS >= 8
		samp[4] = TEXTURE0.Load(uint3(input.Position.x - 1, input.Position.y-1, 0), 0);
		samp[5] = TEXTURE0.Load(uint3(input.Position.x + 1, input.Position.y-1, 0), 0);
		samp[6] = TEXTURE0.Load(uint3(input.Position.x - 1, input.Position.y+1, 0), 0);
		samp[7] = TEXTURE0.Load(uint3(input.Position.x + 1, input.Position.y+1, 0), 0);
#endif

		int validCount = 0;
		float4 validSum = float4(0.0f,0.0f,0.0f,0.0f);
		for (int i = 0; i < SAMPLE_CELLS; i++)
		{
			if (samp[i].a > 0.01f)
			{
				validSum += samp[i];
				validCount++;
			}
		}
		if (validCount > 0)
		{
			col = validSum / (float)validCount;
			col.a = 1.0f;
		}
	}
	
	return col;
}
