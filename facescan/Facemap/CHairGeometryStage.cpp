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

	