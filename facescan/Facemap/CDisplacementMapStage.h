//--------------------------------------------------------------------------------------
// Copyright 2013 Intel Corporation
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
