/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or imlied.
// See the License for the specific language governing permissions and
// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __HEAD_BLEND_STAGE__
#define __HEAD_BLEND_STAGE__

class CPUTModel;
class CPUTRenderTargetColor;
class CPUTMeshDX11;
class CPUTComputeShaderDX11;
struct SBaseHeadInfo;
class CPUTSoftwareMesh;
class CPUTSprite;
#include "CPipeline.h"

struct SHeadBlendStageInput
{
	CPUTRenderParameters *RenderParams;
	SBaseHeadInfo *BaseHeadInfo;
	CPUTSoftwareMesh *DeformedMesh; // mesh that was deformed in the geometry stage
	ID3D11ShaderResourceView *GeneratedFaceColorMap;

	CPUTColor4 BlendColor1;
	CPUTColor4 BlendColor2;
	int3 PostBlendColorize[2];
	int3 PostBlendAdjust[2];
	PostBlendColorMode PostBlendMode;

	int Flags;

	MappingTweaks *Tweaks;
};

struct SHeadBlendStageOutput
{
	CPUTTexture *OutputDiffuse;
};

class CHeadBlendStage
{
public:

	CHeadBlendStage();
	~CHeadBlendStage();

	void Execute(SHeadBlendStageInput *input);
	
	SHeadBlendStageOutput Output;

private:

	void InitResources();
	bool mIsInited;

	CPUTMaterial *mBlendMaterial;
	CPUTMeshDX11 *mRenderMesh;
	CPUTRenderTargetColor *mDiffuseRenderTarget;
	CPUTRenderTargetColor *mDiffuseRenderTarget2;
	CPUTComputeShaderDX11 *mPostComputeShader;
	CPUTMaterial *mSeamFillMaterial;
	CPUTSprite *mFullscreenSprite;
	
};

#endif