/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __OBJ_MODEL__
#define __OBJ_MODEL__

#include "CPUT.h"
#include "ObjLoader.h"
#include <map>
#include "CPUTMath.h"
#include "CPUTSoftwareMesh.h"

class CPUTModel;
class CPUTTexture;

class CFaceModel
{
public:

	CFaceModel();
	~CFaceModel();

	void LoadObjFilename(const std::string &filename, bool landmarks = true);
	CPUTModel *CreateCPUTModel();

	bool LoadLandmarks();

	std::vector<float3> Landmarks;

	float3 AABBMin;
	float3 AABBMax;

	// Unique ID that changes if any part of this model changes
	uint32 GetUniqueId();

	// Updates the unique ID if any data in this mesh changes
	void FlagUpdated();

	CPUTTexture *GetTexture() { return mTexture; }

	CPUTSoftwareMesh *GetMesh() { return &mMesh; }

private:
	std::string mObjFilename;

	// values that were applied to the objmodel after it was loaded
	// in an effort to normalize the mesh
	float mVertScale;
	float3 mVertOffset;

	uint32 mUniqueId;

	CPUTSoftwareMesh mMesh;
	CPUTTexture *mTexture;
	
	bool LoadLandmarks( const std::string &filename, std::vector<float3> &outMap, int formatVersion = 0 );
	
};

#endif // __OBJ_MODEL__