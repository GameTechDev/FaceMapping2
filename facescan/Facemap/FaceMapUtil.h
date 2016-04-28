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
#ifndef __FACEMAPUTIL__
#define __FACEMAPUTIL__

#include "CPUT.h"
#include <vector>

class CPUTSoftwareMesh;
class CMorphTarget;

void LoadOBJToSoftwareMesh(const char *objFilename, CPUTSoftwareMesh *mesh);

void LoadBaseHeadInfo(const char *filename);

void LoadMorphTarget(const char *objFilename, CPUTSoftwareMesh *baseMesh, CMorphTarget *outMorphTarget);
void BuildMorphTarget(CPUTSoftwareMesh *baseMesh, CPUTSoftwareMesh *morphMesh, CMorphTarget *outMorphTarget);


struct MappedVertex
{
	int TriangleIndex; // Index of triangle that each vertex projects onto
	float ClosestDistance; // Closest distance from each vertex to the closest triangle it projects onto
	float3 BarycentricCoordinates; // Barycentric coordinates of point where vertex projects onto triangle
};

void ProjectVerticesOntoTriangles(
	float3 *pSrcPositions,            // Positions of vertices to project
	UINT    srcVertexCount,           // Number of vertices to project
	UINT    triangleCount,            // Number of triangles in mesh projecting onto
	UINT   *pTriangleIndices,         // Vertex indices for each triangle in mesh being projected onto
	float3 *pTriangleVertexPositions, // Position of each vertex in mesh being projected onto
	float3 *pTriangleVertexNormals,   // Normal of each vertex in mesh being projected onto
	MappedVertex *pMapping,           // Output mapping results
    float frontFacingDistanceThreshold,
    float backFacingDistanceThreshold
);

void ProjectMeshVerticesOntoMeshTriangles(
    CPUTSoftwareMesh *meshVertices,
    CPUTSoftwareMesh *meshTriangles,
    std::vector<MappedVertex> &outMapped,
    float frontFacingDistanceThreshold = FLT_MAX,
    float backFacingDistanceThreshold = FLT_MAX
);
#endif