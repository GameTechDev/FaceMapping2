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
#include "CHairGeometryStage.h"
#include "../SampleUtil.h"

CHairGeometryStage::CHairGeometryStage()
{

}
CHairGeometryStage::~CHairGeometryStage()
{

}

void CHairGeometryStage::Execute(SHairGeometryStageInput *input)
{
	if (mMappedHairVerts.size() != input->Hair->GetVertCount() || input->ClearCachedProjections)
	{
		ProjectMeshVerticesOntoMeshTriangles(input->Hair, input->BaseHead, mMappedHairVerts, 8.0f, 1.0f);
	}

	DeformedHair.CopyFrom(input->Hair);

	// **************************************
	// Project the hair to the morphed head
	// Move the hair vertices to match the morphed head
	// foreach hair vertex vertHair
	//   newVertHair = sum( bary[0..2] * vHeadDeformed[0..2] + d[vertHair] * normalHead[0..2]
	int hairVertCount = (int)mMappedHairVerts.size();
    UINT32 *pIndices   = input->DeformedHead->IB;
    float3 *pPositions = input->DeformedHead->Pos;
    float3 *pNormals   = input->DeformedHead->Normal;

	for (int vIdx = 0; vIdx < hairVertCount; vIdx++)
	{
		MappedVertex *vMap = &mMappedHairVerts[vIdx];
		if (vMap->ClosestDistance == FLT_MAX) continue; // vertex misses mesh

		UINT   hIdx = vMap->TriangleIndex;

		// Vertex indices from triangle index
		UINT   i0 = pIndices[hIdx * 3 + 0];
		UINT   i1 = pIndices[hIdx * 3 + 1];
		UINT   i2 = pIndices[hIdx * 3 + 2];

		// Vertex positions
		float3 v0 = pPositions[i0];
		float3 v1 = pPositions[i1];
		float3 v2 = pPositions[i2];

		// Vertex normals
		float3 n0 = pNormals[i0];
		float3 n1 = pNormals[i1];
		float3 n2 = pNormals[i2];

		// Closest distance from hair vertex to head triangle
		float dd = vMap->ClosestDistance;

        float3 barys  = vMap->BarycentricCoordinates;
        float3 pos    = barys.x*v0 + barys.y*v1 + barys.z*v2;
        float3 normal = barys.x*n0 + barys.y*n1 + barys.z*n2;
        
		DeformedHair.Pos[vIdx] = pos + dd*normal;
	}
}

	