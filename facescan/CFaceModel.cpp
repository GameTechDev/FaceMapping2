/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "CFaceModel.h"
#include "CPUTModel.h"
#include "CPUTMesh.h"
#include "CPUTMaterial.h"
#include "SampleUtil.h"
#include "CPUTTexture.h"

static uint32 gFaceScanUniqueId = 0;

CFaceModel::CFaceModel() :
mObjFilename(""),
mTexture(NULL)
{
	FlagUpdated();
}

CFaceModel::~CFaceModel()
{
	SAFE_RELEASE(mTexture);
}

uint32 CFaceModel::GetUniqueId() { return mUniqueId; }
void CFaceModel::FlagUpdated() { mUniqueId = gFaceScanUniqueId++; }

void CFaceModel::LoadObjFilename(const std::string &filename, bool landmarks)
{
	AABBMin = float3(10000.0f, 10000.0f, 10000.0f);
	AABBMax = -AABBMin;

	mObjFilename = filename;
	{
		tObjModel objModel;
		objLoader(filename.c_str(), objModel);
		CopyOBJDataToSoftwareMesh(&objModel, &mMesh);
	}

	float boxSize = 8.0f;
	int vertCount = mMesh.GetVertCount();
	if (vertCount > 0)
	{
		float3 vmin = mMesh.Pos[0];
		float3 vmax = mMesh.Pos[0];

		for (int i = 0; i < vertCount; i++)
		{
			vmin = Min(vmin, mMesh.Pos[i]);
			vmax = Max(vmax, mMesh.Pos[i]);
		}

		float3 center = (vmax + vmin) / 2.0f;
		float3 dim = vmax - vmin;

		float maxDim = floatMax(dim.x, floatMax(dim.y, dim.z));

		mVertOffset = -center;
		mVertScale = boxSize / maxDim;

		// center to origin, scale, and rotate
		// 
		for (int i = 0; i < vertCount; i++)
		{
			float3 *pos = &mMesh.Pos[i];
			*pos -= center;
			*pos *= mVertScale;
			Swap(pos->z, pos->y);

			// save scale and rotate from RSSDK format. Z is up in rssdk
			mMesh.Tex[i].y = 1.0f - mMesh.Tex[i].y;

			AABBMin = Min(*pos, AABBMin);
			AABBMax = Max(*pos, AABBMax);
		}
	}
	LoadLandmarks();

	// load texture
	SAFE_RELEASE(mTexture);
	std::string fnString = mObjFilename;
	int lastindex = (int)fnString.find_last_of(".");
	std::string rawname = fnString.substr(0, lastindex);
	std::string textureName = rawname.append("image1.png");
	mTexture = CPUTTexture::Create(std::string("facetexture"), textureName, false);

	FlagUpdated();
}

CPUTModel *CFaceModel::CreateCPUTModel()
{
	ProfileBlockScoped block("LoadCPUTModel");
	
	CPUTModel *model = CPUTModel::Create();
	CPUTMesh *mesh = CPUTMesh::Create();
	mMesh.CopyToDX11Mesh((CPUTMeshDX11*)mesh);

	model->mpMaterialCount = new int[1];
	model->mpMaterial = new CPUTMaterial**[1];
	model->mpMaterial[0] = new CPUTMaterial*[1];
	model->mpMaterial[0][0] = NULL;
	model->mpMaterialCount[0] = 1;
	
	model->SetMeshes(&mesh, 1);

	return model;
}

bool CFaceModel::LoadLandmarks(const std::string &filename, std::vector<float3> &outList, int formatVersion)
{
	char line[1024];
	
	FILE *file = fopen(filename.c_str(), "r");

	if (file == NULL)
		return false;
	
	char toRemove[] = { '\t', ' ', '{', '}', '[', ']', '\"', '\n', ',', 'x', 'y', 'z' };
	const char *toRemoveStrings[] = { "Name", "Confidence", "Position" };
	while (fgets(line, sizeof(line), file))
	{
		bool hasConfidence = strstr(line, "Confidence") != NULL;
		for (int i = 0; i < ARRAYSIZE(toRemoveStrings); i++)
		{
			char *loc = line;
			while (loc = strstr(loc, toRemoveStrings[i]))
			{
				int toRemoveLen = (int)strlen(toRemoveStrings[i]);
				memmove(loc, loc + toRemoveLen, 1 + strlen(loc + toRemoveLen));
			}
		}
		{
			int writeLoc = 0;
			char sLine[1024];
			
			for (int i = 0; i < 1024 && line[i] != 0; i++)
			{
				char c = line[i];
				bool skip = false;
				for (int j = 0; j < ARRAYSIZE(toRemove); j++)
				{
					if (toRemove[j] == c)
						skip = true;
				}
				if (c == ':')
					c = ' ';
				if (!skip)
					sLine[writeLoc++] = c;
			}
			sLine[writeLoc] = 0;

			char landmarkName[256];
			float3 pos;
			int confidence = 100;
			bool scanValid = true;
			if (hasConfidence)
			{
				int result = sscanf_s(sLine, "%s %d %f %f %f", landmarkName, sizeof(landmarkName), &confidence, &pos.x, &pos.y, &pos.z);
				scanValid = result == 5;
			}
			else
			{
				int result = sscanf_s(sLine, "%s %f %f %f", landmarkName, sizeof(landmarkName), &pos.x, &pos.y, &pos.z);
				scanValid = result == 4;
			}
			
			if (scanValid)
			{
				pos += mVertOffset;
				pos *= mVertScale;
				floatSwap(&pos.y, &pos.z);
				outList.push_back(pos);
			}
		}
	}
	fclose(file);
	
	FlagUpdated();

	return outList.size() != 0;
}

bool CFaceModel::LoadLandmarks()
{
	Landmarks.clear();
	std::string fnString = mObjFilename;
	int lastindex = (int)fnString.find_last_of(".");
	std::string rawname = fnString.substr(0, lastindex);
	std::string jsonName = rawname.append(".json");

	if (LoadLandmarks(jsonName, Landmarks, 0))
		return true;
	return LoadLandmarks(jsonName, Landmarks, 1);
}

