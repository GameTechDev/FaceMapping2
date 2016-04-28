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