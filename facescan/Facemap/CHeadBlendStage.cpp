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
#include "CPUT.h"
#include "CPUTRenderTarget.h"
#include "CPUTModel.h"
#include "CHeadBlendStage.h"
#include "CPUTModel.h"
#include "CPUTMaterial.h"
#include "CPUTTexture.h"
#include "CPUTTextureDX11.h"
#include "CPUTBuffer.h"
#include "CPUTRenderParams.h"
#include "CPUTSoftwareMesh.h"
#include "CPipeline.h"
#include "CPUTComputeShaderDX11.h"
#include "../SampleUtil.h"
#include "CPUTSprite.h"


#define COLORMAP_WIDTH 4096
#define COLORMAP_HEIGHT 4096

struct BlendShaderConstantBuffer
{
	float4 HeadColor1;
	float4 HeadColor2;
	float4 PostBlendHSL1;
	float4 PostBlendHSL2;
	float UseFaceColor;
	float PostBlendMode;
	float OtherHeadBlend;
};

CHeadBlendStage::CHeadBlendStage() :
mDiffuseRenderTarget(NULL),
mDiffuseRenderTarget2(NULL),
mBlendMaterial(NULL),
mRenderMesh(NULL),
mFullscreenSprite(NULL),
mSeamFillMaterial(NULL),
mIsInited(false)
{
	ZeroMemory(&Output, sizeof(Output));
}

CHeadBlendStage::~CHeadBlendStage()
{
	SAFE_DELETE(mDiffuseRenderTarget);
	SAFE_DELETE(mDiffuseRenderTarget2);
	SAFE_RELEASE(mBlendMaterial);
	SAFE_DELETE(mFullscreenSprite);
	SAFE_RELEASE(mSeamFillMaterial);
	SAFE_RELEASE(mRenderMesh);
	SAFE_RELEASE(Output.OutputDiffuse);
}

void CHeadBlendStage::InitResources()
{
	if (mIsInited)
		return;

	mDiffuseRenderTarget = CPUTRenderTargetColor::Create();
	mDiffuseRenderTarget->CreateRenderTarget("$Sculpt", COLORMAP_WIDTH, COLORMAP_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	mDiffuseRenderTarget2 = CPUTRenderTargetColor::Create();
	mDiffuseRenderTarget2->CreateRenderTarget("$Sculpt2", COLORMAP_WIDTH, COLORMAP_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

	CPUTAssetLibrary *pAssetLibrary = CPUTAssetLibrary::GetAssetLibrary();
	pAssetLibrary->SetRootRelativeMediaDirectory("MyAssets");
	mBlendMaterial = pAssetLibrary->GetMaterial("sculpt");
	mSeamFillMaterial = pAssetLibrary->GetMaterial("seamfill");
	mFullscreenSprite = CPUTSprite::Create(-1.0f, -1.0f, 2.0f, 2.0f, NULL);
	
	mRenderMesh = CPUTMeshDX11::Create();

	mIsInited = true;
}


void CHeadBlendStage::Execute(SHeadBlendStageInput *input)
{
	InitResources();

	input->DeformedMesh->CopyToDX11Mesh(mRenderMesh);

	float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	mDiffuseRenderTarget->SetRenderTarget(*input->RenderParams, NULL, 0, black, true);

	BlendShaderConstantBuffer cb;

	SetCodeTexture(0, input->GeneratedFaceColorMap);
	SetCodeTexture(1, input->BaseHeadInfo->Textures[eBaseHeadTexture_ControlMap_Color]);
	SetCodeTexture(2, input->BaseHeadInfo->Textures[eBaseHeadTexture_FeatureMap]);
	SetCodeTexture(3, input->BaseHeadInfo->Textures[eBaseHeadTexture_ColorTransfer]);
	SetCodeTexture(4, input->BaseHeadInfo->Textures[eBaseHeadTexture_Skin]);
	SetCodeTexture(5, input->Tweaks->OtherHeadTexture);
	
	cb.OtherHeadBlend = input->Tweaks->OtherHeadBlend;
	
	// convert to linear RGB
	cb.HeadColor1.x = pow(input->BlendColor1.r, 2.2f);
	cb.HeadColor1.y = pow(input->BlendColor1.g, 2.2f);
	cb.HeadColor1.z = pow(input->BlendColor1.b, 2.2f);
	cb.HeadColor2.x = pow(input->BlendColor2.r, 2.2f);
	cb.HeadColor2.y = pow(input->BlendColor2.g, 2.2f);
	cb.HeadColor2.z = pow(input->BlendColor2.b, 2.2f);

	int3 c1 = (input->PostBlendMode == PostBlendColorMode_Adjust) ? input->PostBlendAdjust[0] : input->PostBlendColorize[0];
	int3 c2 = (input->PostBlendMode == PostBlendColorMode_Adjust) ? input->PostBlendAdjust[1] : input->PostBlendColorize[1];
	cb.PostBlendHSL1 = float4(c1.x/360.0f, c1.y/100.0f, c1.z/100.0f, 1.0f);
	cb.PostBlendHSL2 = float4(c2.x/360.0f, c2.y/100.0f, c2.z/100.0f, 1.0f);

	cb.UseFaceColor = (input->Flags & PIPELINE_FLAG_SkipFaceColorBlend) ? 0.0f : 1;
	cb.PostBlendMode = (float)input->PostBlendMode;
	
	CPUTBuffer *pBuffer = input->RenderParams->mpPerModelConstants;
	pBuffer->SetData(0, sizeof(BlendShaderConstantBuffer), &cb);
	
	mBlendMaterial->SetRenderStates();
	CPUTInputLayoutCache::GetInputLayoutCache()->Apply(mRenderMesh, mBlendMaterial);
	mRenderMesh->Draw();

	mDiffuseRenderTarget->RestoreRenderTarget(*input->RenderParams);

	CPUTRenderTargetColor *outputRT = mDiffuseRenderTarget;
	if ((input->Flags & PIPELINE_FLAG_SkipColorSeamFill) == 0)
	{
		mDiffuseRenderTarget2->SetRenderTarget(*input->RenderParams);
		SetCodeTexture(0, mDiffuseRenderTarget->GetColorResourceView());
		mFullscreenSprite->DrawSprite(*input->RenderParams, *mSeamFillMaterial);
		mDiffuseRenderTarget2->RestoreRenderTarget(*input->RenderParams);
		outputRT = mDiffuseRenderTarget2;
	}
		
	// Create a new texture from the render target at override the material texture with it
	if (Output.OutputDiffuse == NULL)
	{
		Output.OutputDiffuse = CPUTTextureDX11::CreateFromRenderTarget(std::string("facediffuse"), outputRT);
	}
	else
	{
		((CPUTTextureDX11*)Output.OutputDiffuse)->CopyFromRenderTarget(outputRT);
	}
	
}