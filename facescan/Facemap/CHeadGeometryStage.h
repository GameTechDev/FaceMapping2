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
#ifndef __HEAD_GEOMETRY_STAGE__
#define __HEAD_GEOMETRY_STAGE__

#include <vector>
#include "CPUTSoftwareMesh.h"
#include "FaceMapUtil.h"

class CPUTSoftwareTexture;
class CPUTModel;
class CPUTMeshDX11;
struct CDisplacementMapStageOutput;
struct SBaseHeadInfo;
class CPUTSoftwareMesh;

class MappingTweaks;
struct HeadVertex;
struct HeadProjectionInfo;

struct SHeadGeometryStageInput
{
	CPUTSoftwareTexture *DisplacementMap;
	CDisplacementMapStageOutput *DisplacementMapInfo;
	SBaseHeadInfo *BaseHeadInfo;
	MappingTweaks *Tweaks;
	bool ClearCachedProjections;
};


class CHeadGeometryStage
{
public:

	CHeadGeometryStage();
	~CHeadGeometryStage();

	void Execute(SHeadGeometryStageInput *input);

	CPUTSoftwareMesh DeformedMesh;

	std::vector<float3> MorphedHeadLandmarks;
	std::vector<int> LandmarkMeshVertexToLandmarkIndex;

	CPUTSoftwareMesh MorphedLandmarkMesh;

private:

	void UpdateHeadProjectionInfo(CDisplacementMapStageOutput *dispMapInfo, SBaseHeadInfo *headInfo, MappingTweaks *tweaks, HeadProjectionInfo *outProjInfo);

	// CPU cached vertex data
	CPUTModel *mCachedModel;

	CPUTModel *mCloneSource; // the source CPUTModel that was cloned
	CPUTMeshDX11 *mCloneMesh;

	std::vector<MappedVertex> mMappedFaceVertices;

	CPUTCamera *mMapProjectionCamera;
};

#endif