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