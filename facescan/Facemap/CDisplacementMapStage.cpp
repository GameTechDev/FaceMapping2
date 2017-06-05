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
#include "CDisplacementMapStage.h"
#include "CPUTCamera.h"
#include "CPUTTexture.h"
#include "CPUTModel.h"
#include "CPUT_DX11.h"
#include "CPipeline.h"
#include "../CFaceModel.h"

const UINT  FACE_DISPLACEMENT_WIDTH_HEIGHT = 1024;

CDisplacementMapStage::CDisplacementMapStage():
mCPUTFaceModel(NULL),
mpDepthBuffer(NULL),
mFaceModelId(0)
{
	mCamera = CPUTCamera::Create(CPUT_ORTHOGRAPHIC);
	mCamera->SetPosition(0.0f, 0.0f, -10.0f);
	mCamera->LookAt(0.0f, 0.0f, 0.0f);

	mCamera->SetFarPlaneDistance(20);
	mCamera->SetNearPlaneDistance(1.0f);
	mCamera->SetAspectRatio(1.0f);
	mCamera->SetWidth(10);
	mCamera->SetHeight(10);
	mCamera->Update();
}

CDisplacementMapStage::~CDisplacementMapStage()
{
	SAFE_RELEASE(mCPUTFaceModel);
	SAFE_DELETE(mCamera);
	SAFE_DELETE(mpDepthBuffer);
	SAFE_DELETE(Output.ColorMap);
	SAFE_DELETE(Output.DepthMap);
}

void CDisplacementMapStage::ResizeTextures(int width, int height, bool force)
{
	if (force || Output.ColorMap == NULL || (Output.ColorMap->GetWidth() != width) || (Output.ColorMap->GetHeight() != height))
	{
		SAFE_DELETE(Output.ColorMap);
		Output.ColorMap = CPUTRenderTargetColor::Create();
		Output.ColorMap->CreateRenderTarget("FaceColor", width, height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	}
	if (force || Output.DepthMap == NULL || (Output.DepthMap->GetWidth() != width) || (Output.DepthMap->GetHeight() != height))
	{
		SAFE_DELETE(Output.DepthMap);
		Output.DepthMap = CPUTRenderTargetColor::Create();
		Output.DepthMap->CreateRenderTarget("FaceDisplacement", width, height, DXGI_FORMAT_R32_FLOAT);
	}
	if (force || mpDepthBuffer == NULL || (mpDepthBuffer->GetWidth() != width) || (mpDepthBuffer->GetHeight() != height))
	{
		SAFE_DELETE(mpDepthBuffer);
		mpDepthBuffer = CPUTRenderTargetDepth::Create();
		mpDepthBuffer->CreateRenderTarget("FaceDisplacementDepth", width, height, DXGI_FORMAT_D32_FLOAT);
	}
}

void CDisplacementMapStage::CreateFaceCPUTModel(CFaceModel *faceModel, bool force)
{
	if (force || mCPUTFaceModel == NULL || mFaceModelId != faceModel->GetUniqueId())
	{
		SAFE_RELEASE(mCPUTFaceModel);
		
		mCPUTFaceModel = faceModel->CreateCPUTModel();

		CPUTAssetLibrary *pAssetLibrary = CPUTAssetLibrary::GetAssetLibrary();
		pAssetLibrary->SetRootRelativeMediaDirectory("MyAssets");
		std::string matName = pAssetLibrary->GetMaterialDirectoryName();
		CPUTFileSystem::CombinePath(matName, "displace_map_render.mtl", &matName);

		CPUTMaterial *material = CPUTMaterial::Create(matName);
		material->OverridePSTexture(0, faceModel->GetTexture());
		mCPUTFaceModel->SetMaterial(0, &material, 1);
		material->Release();

		mFaceModelId = faceModel->GetUniqueId();
	}
}

bool CDisplacementMapStage::Execute(SDisplacementMapStageInput *input)
{
	ResizeTextures(FACE_DISPLACEMENT_WIDTH_HEIGHT, FACE_DISPLACEMENT_WIDTH_HEIGHT);
	CreateFaceCPUTModel(input->FaceModel);

	CPUTRenderParameters *renderParams = input->RenderParams;
	CPUTCamera *pLastCamera = renderParams->mpCamera;

	renderParams->mpCamera = mCamera;
	float black[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	// Render the displacement map - set it here only so we can use the restore feature at the end of this file
	Output.DepthMap->SetRenderTarget(*renderParams, mpDepthBuffer, 0, black, true, 0.0f);

	// CPUT doesn't support MRT, go directly to DX
	ID3D11RenderTargetView *rtvs[2];
	rtvs[0] = Output.ColorMap->GetColorBufferView();
	rtvs[1] = Output.DepthMap->GetColorBufferView();

	for (int i = 0; i < 2; i++)
	{
		CPUT_DX11::GetContext()->ClearRenderTargetView(rtvs[i], black);
	}

	float3 minFaceZone = input->FaceModel->AABBMin;
	float3 maxFaceZone = input->FaceModel->AABBMax;

	float nearClippingPlaneOffset = 2.0f;

	Output.DepthMap_ZRange = (maxFaceZone.z - minFaceZone.z) + nearClippingPlaneOffset;
	Output.DepthMap_ZMeshStart = 1.0f - (nearClippingPlaneOffset / Output.DepthMap_ZRange);
	float cameraDistance = 10.0f;
	float nearPlane = cameraDistance + minFaceZone.z - nearClippingPlaneOffset;

	mCamera->SetPosition(0.0f, 0.0f, -cameraDistance);
	mCamera->LookAt(0.0f, 0.0f, 0.0f);
	mCamera->SetNearPlaneDistance(nearPlane);
	mCamera->SetFarPlaneDistance(nearPlane + Output.DepthMap_ZRange);
	mCamera->Update();

	CPUT_DX11::GetContext()->OMSetRenderTargets(2, rtvs, mpDepthBuffer->GetDepthBufferView());

	float4x4 modelMatrix = float4x4Identity();
	modelMatrix = modelMatrix * float4x4RotationX(input->FacePitch);
	modelMatrix = modelMatrix * float4x4RotationY(input->FaceYaw);
	modelMatrix = modelMatrix * float4x4RotationZ(input->FaceRoll);
	mCPUTFaceModel->SetParentMatrix(modelMatrix);

	mCPUTFaceModel->Render(*renderParams, 0);
	Output.DepthMap->RestoreRenderTarget(*renderParams);

	// calculate mapping details
	float4x4 displacementMapMatrix = *mCamera->GetViewMatrix() * *mCamera->GetProjectionMatrix();

	float4 leftEye = float4(input->FaceModel->Landmarks[kLandmarkIndex_LeftEyeAnchor], 1.0f) * modelMatrix;
	float4 rightEye = float4(input->FaceModel->Landmarks[kLandmarkIndex_RightEyeAnchor], 1.0f) * modelMatrix;
	Output.EyeDistance_FaceModelSpace = abs(leftEye.x - rightEye.x);
	leftEye = leftEye * displacementMapMatrix;
	rightEye = rightEye * displacementMapMatrix;
	Output.EyeDistance_MapSpace = abs(leftEye.x - rightEye.x);

	float4 anchor = (rightEye + leftEye) / 2;
	Output.Anchor_MapSpace = float2(anchor.x, anchor.y);

	float4x4 faceLandmarkToMapLandmark = modelMatrix * displacementMapMatrix;

	Output.MapLandmarks.clear();
	for (int i = 0; i < (int)input->FaceModel->Landmarks.size(); i++)
	{
		float4 pos = float4(input->FaceModel->Landmarks[i], 1.0f) * faceLandmarkToMapLandmark;
		Output.MapLandmarks.push_back(float2(pos.x, pos.y));
	}

	return true;
}