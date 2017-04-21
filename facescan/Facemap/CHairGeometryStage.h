/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __HAIR_GEOMETRY_STAGE__
#define __HAIR_GEOMETRY_STAGE__

#include "CPUTSoftwareMesh.h"
#include "FaceMapUtil.h"
#include <vector>

class CPUTModel;
class CPUTRenderTargetColor;
class CPUTMeshDX11;
struct SBaseHeadInfo;
class CPUTSoftwareMesh;

struct SHairGeometryStageInput
{
	CPUTSoftwareMesh *Hair;
	CPUTSoftwareMesh *BaseHead;
	CPUTSoftwareMesh *DeformedHead;

	bool ClearCachedProjections;
};

class CHairGeometryStage
{
public:

	CHairGeometryStage();
	~CHairGeometryStage();

	void Execute(SHairGeometryStageInput *input);

	CPUTSoftwareMesh DeformedHair;

private:

	std::vector<MappedVertex> mMappedHairVerts;
};

#endif // __HAIR_GEOMETRY_STAGE__