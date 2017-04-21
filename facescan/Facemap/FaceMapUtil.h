/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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