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
#ifndef __DISPLACEMENT_MAP_STAGE__
#define __DISPLACEMENT_MAP_STAGE__

#include "CPUT.h"
#include "CPUTMath.h"
#include "CPUTRenderParams.h"
#include <vector>

class CPUTModel;
class CPUTRenderTarget;
class CPUTCamera;
class CPUTRenderTargetColor;
class CPUTRenderTargetDepth;
class CFaceModel;

struct SDisplacementMapStageInput
{
	CFaceModel *FaceModel;
	
	CPUTRenderParameters *RenderParams;

	// Transform to apply before rendering
	float FacePitch;
	float FaceYaw;
	float FaceRoll;
};

struct CDisplacementMapStageOutput
{
	// distance between the left and right eye center in the face model space and map space
	float EyeDistance_FaceModelSpace;
	float EyeDistance_MapSpace;

	// coordinates of the anchor point projected onto the map textures x: -1 to 1, y: -1 to 1
	float2 Anchor_MapSpace;

	// the depth values in the depth image will be in range 0-1 DepthMap_ZRange is the Z distance that
	// range represents in world space. AKA: farClip - nearClip
	float DepthMap_ZRange;

	// The z value at which the face mesh starts. The near clipping plane is shifted away from the face to avoid artifacts
	// with near plane clipping
	float DepthMap_ZMeshStart;

	CPUTRenderTargetColor *DepthMap;
	CPUTRenderTargetColor *ColorMap;

	std::vector<float2> MapLandmarks;

	CDisplacementMapStageOutput() :
		EyeDistance_FaceModelSpace(0.0f),
		EyeDistance_MapSpace(0.0f),
		Anchor_MapSpace(0.0f),
		DepthMap_ZRange(0.0f),
		DepthMap_ZMeshStart(0.0f),
		DepthMap(NULL),
		ColorMap(NULL)
	{
	}
};

class CDisplacementMapStage
{
public:
	CDisplacementMapStage();
	~CDisplacementMapStage();

	bool Execute(SDisplacementMapStageInput *input);

	CDisplacementMapStageOutput Output;
private:

	void ResizeTextures(int width, int height, bool force = false);
	void CreateFaceCPUTModel(CFaceModel *FaceModel, bool force = false);

	CPUTRenderTargetDepth *mpDepthBuffer;
	CPUTCamera *mCamera;
	CPUTModel *mCPUTFaceModel;
	uint32 mFaceModelId;
	
};


#endif
