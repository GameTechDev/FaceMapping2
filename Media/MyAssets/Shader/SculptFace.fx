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


Texture2D FaceColorMap; // face color from generated face color map
Texture2D ColorControlMap; // control blending between the head color and the face color
Texture2D FeatureMap; // greyscale detail map to give texture to the head color
Texture2D ColorTransferMap; // controls blending between the two user selected colors
Texture2D SkinMask; // used to mask out eyes and lips so they're unaffected by post blending effects
Texture2D OtherHeadTexture; // used when blending between the current head and a previously stored head

SamplerState SAMPLER0 : register(s0);

cbuffer cbPerModelValues
{
	// User selected head colors
	float4 HeadColor1;
	float4 HeadColor2;

	// Parameters for blending post processing.
	float4 PostBlendHSL1;
	float4 PostBlendHSL2;

	float UseFaceColor; // 1 blend face in, 0 head only (for debug purposes)
	float PostBlendMode; // 0 for none, 1 for colorize, 2 for adjust
	float OtherHeadBlend;
};

struct VS_INPUT
{
	float2 Uv1 : TEXCOORD0; // head model texture coordinates. All the artists authored maps are in this space
	float2 Uv2 : TEXCOORD1; // associated coorinated on the face color map. Calculated in geometry stage
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Uv1 : TEXCOORD0;
	float2 Uv2 : TEXCOORD1; 
};

float RBGToLuminosity(float3 rgb)
{
	return rgb.r * 0.299 + rgb.g * 0.587 + rgb.b * 0.114;
}

float3 RGBtoHSL(float3 color)
{
	float M = max(max(color.r, color.b), color.g);
	float m = min(min(color.r, color.b), color.g);
	float C = M - m;

	float lightness = (M + m) * 0.5;
	float hue = 0.0f;
	float sat = 0.0f;
	if (C > 0.00001)
	{
		if (M == color.r)
			hue = ((color.g - color.b) / C) % 6;
		else if (M == color.g)
			hue = ((color.b - color.r) / C) + 2.0f;
		else
			hue = ((color.b - color.r) / C) + 4.0f;
		
		hue /= 6.0f;
		sat = C / (1 - abs(2 * lightness-1));
	}

	return float3(hue, sat, lightness);
}

float3 HSLtoRGB(in float3 HSL)
{
	float hue = HSL.x;
	float sat = HSL.y;
	float lightness = HSL.z;
	float C = (1 - abs(2 * lightness - 1)) * sat;
	
	hue *= 6;
	float X = C * (1 - abs((hue % 2) - 1));
	float3 rgb = float3(0, 0, 0);
	if (hue < 1.0f)
		rgb = float3(C, X, 0);
	else if ( hue < 2.0f )
		rgb = float3(X, C, 0);
	else if (hue < 3.0f)
		rgb = float3(0, C, X);
	else if (hue < 4.0f)
		rgb = float3(0, X, C);
	else if (hue < 5.0f)
		rgb = float3(X, 0, C);
	else if (hue < 6.0f)
		rgb = float3(C, 0, X);

	float m = lightness - 0.5 * C;
	rgb = rgb + float3(m, m, m);

	return rgb;
}

// ********************************************************************************************************
PS_INPUT VSMain(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Uv1 = input.Uv1;
	output.Uv2 = input.Uv2;

	output.Pos.x = input.Uv1.x * 2.0 - 1.0;
	output.Pos.y = (1.0 - input.Uv1.y) * 2.0 - 1.0;
	output.Pos.z = 0.5;
	output.Pos.w = 1.0;

	return output;
}

float3 AdjustRGB(float3 rgb, float3 adjustHSL)
{
	float3 hsl = RGBtoHSL(rgb);
	hsl.x = (hsl.x + adjustHSL.x + 1.0f) % 1.0f; // hue
	hsl.y = clamp(hsl.y * (1.0f + adjustHSL.y), 0.0f, 1.0f); // sat
	hsl.z = clamp(hsl.z * (1.0f + adjustHSL.z), 0.0f, 1.0f); // l
	return HSLtoRGB(hsl);
}

float3 ColorizeRGB(float3 rgb, float3 colorizeHSL)
{
	float3 hsl = float3(0.0f, 0.0f, RBGToLuminosity(rgb));
	hsl.x = colorizeHSL.x;
	hsl.y = colorizeHSL.y;
	hsl.z = clamp(hsl.z * (1.0f + colorizeHSL.z), 0.0f, 1.0f);
	return HSLtoRGB(hsl);
}

// ********************************************************************************************************
float4 PSMain(PS_INPUT input)  : SV_Target
{
	float3 featureMap = FeatureMap.Sample(SAMPLER0, input.Uv1).rgb;
	float colorTransferWeight = ColorTransferMap.Sample(SAMPLER0, input.Uv1).r;
	float3 scanColor = FaceColorMap.Sample(SAMPLER0, input.Uv2).rgb;
	float controlWeight = ColorControlMap.Sample(SAMPLER0, input.Uv1).r;

	// use the color transfer map to choose the color to use for the head
	float3 headColor = lerp(HeadColor1.xyz, HeadColor2.xyz, colorTransferWeight);

	// a feature map value of 0.5 is neutral. Anything above 0.5 will brighten the color and below 0.5 will darken it
	headColor = headColor * (featureMap * 2.0f);

	// blend between player's face color and model color based on cotrol weight
	float3 diffuse = lerp(headColor, scanColor, controlWeight * UseFaceColor);
	
	// Apply optional post blending
	if (PostBlendMode > 1.001f) // Adjust
	{
		float skinMask = SkinMask.Sample(SAMPLER0, input.Uv1).r;
		float3 rgbColor1 = AdjustRGB(diffuse.rgb, PostBlendHSL1.xyz);
		float3 rgbColor2 = AdjustRGB(diffuse.rgb, PostBlendHSL2.xyz);
		float3 adjustedColor = lerp(rgbColor1, rgbColor2, colorTransferWeight);
		diffuse = lerp(diffuse, adjustedColor, skinMask);
	}
	else if (PostBlendMode > 0.0001f) // Colorize
	{
		float skinMask = SkinMask.Sample(SAMPLER0, input.Uv1).r;
		float3 rgb1 = ColorizeRGB(diffuse.rgb, PostBlendHSL1.xyz);
		float3 rgb2 = ColorizeRGB(diffuse.rgb, PostBlendHSL2.xyz);
		float3 adjustedColor = lerp(rgb1, rgb2, colorTransferWeight);
		diffuse = lerp(diffuse, adjustedColor, skinMask);
	}

	// Blend in the color from a previously built head if necessary
	if (OtherHeadBlend > 0.0f)
		diffuse = lerp(diffuse, OtherHeadTexture.Sample(SAMPLER0, input.Uv1).rgb, OtherHeadBlend);

	return float4(diffuse, 1.0f); 
}
