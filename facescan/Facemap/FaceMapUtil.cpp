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
#include "FaceMapUtil.h"
#include "CPUTSoftwareMesh.h"
#include "CPipeline.h"
#include "../ObjLoader.h"

void LoadOBJToSoftwareMesh(const char *objFilename, CPUTSoftwareMesh *mesh)
{
	tObjModel objModel;
	objLoader(objFilename, objModel);

	mesh->UpdateIndexCount((int)objModel.m_indices.size());
	assert(sizeof(ObjIndexInt) == sizeof(uint32_t));
	memcpy(mesh->IB, &objModel.m_indices[0], sizeof(uint32_t) * objModel.m_indices.size());

	int vertCount = (int)objModel.m_vertices.size();
	mesh->UpdateVertexCount(vertCount);
	mesh->AddComponent(eSMComponent_Position);
	mesh->AddComponent(eSMComponent_Normal);
	mesh->AddComponent(eSMComponent_Tex1);
	for (int i = 0; i < vertCount; i++)
	{
		mesh->Pos[i] = float3(objModel.m_vertices[i].x, objModel.m_vertices[i].y, objModel.m_vertices[i].z);
		mesh->Normal[i] = float3(objModel.m_vertices[i].nx, objModel.m_vertices[i].ny, objModel.m_vertices[i].nz);
		mesh->Tex[i] = float2(objModel.m_vertices[i].u, objModel.m_vertices[i].v);
	}
}

void LoadMorphTarget(const char *objFilename, CPUTSoftwareMesh *baseMesh, CMorphTarget *outMorphTarget)
{
	outMorphTarget->MorphVerts.clear();

	CPUTSoftwareMesh mesh;
	LoadOBJToSoftwareMesh(objFilename, &mesh);

	assert(baseMesh->GetVertCount() == mesh.GetVertCount());

	int vertCount = baseMesh->GetVertCount();
	for (int i = 0; i < vertCount; i++)
	{
		if (baseMesh->Pos[i] != mesh.Pos[i])
		{
			MorphTargetVertex vertex;
			vertex.VertIndex = i;
			vertex.Delta = mesh.Pos[i] - baseMesh->Pos[i];
			outMorphTarget->MorphVerts.push_back(vertex);
		}
	}

}

void BuildMorphTarget(CPUTSoftwareMesh *baseMesh, CPUTSoftwareMesh *morphMesh, CMorphTarget *outMorphTarget)
{
	outMorphTarget->MorphVerts.clear();

	assert(baseMesh->GetVertCount() == morphMesh->GetVertCount());

	int vertCount = baseMesh->GetVertCount();
	for (int i = 0; i < vertCount; i++)
	{
		if (baseMesh->Pos[i] != morphMesh->Pos[i] && baseMesh->Normal[i] != morphMesh->Normal[i])
		{
			MorphTargetVertex vertex;
			vertex.VertIndex = i;
			vertex.Delta = morphMesh->Pos[i] - baseMesh->Pos[i];
			vertex.NormalDelta = morphMesh->Normal[i] - baseMesh->Normal[i];
			outMorphTarget->MorphVerts.push_back(vertex);
		}
	}

}


//-----------------------------------------------------------------------------
void ProjectVerticesOntoTriangles(
	float3 *pSrcPositions,            // Positions of vertices to project
	UINT    srcVertexCount,           // Number of vertices to project
	UINT    triangleCount,            // Number of triangles in mesh projecting onto
	UINT   *pTriangleIndices,         // Vertex indices for each triangle in mesh being projected onto
	float3 *pTriangleVertexPositions, // Position of each vertex in mesh being projected onto
	float3 *pTriangleVertexNormals,   // Normal of each vertex in mesh being projected onto
	MappedVertex *pMapping,           // Output mapping results
    float   frontFacingDistanceThreshold, // Don't map vertices to front-facing triangles further than this distance away
    float   backFacingDistanceThreshold   // Don't map vertices to back-facing triangles further than this distance away
	){
	// Determine which triangle that each vertex projects onto
	for (UINT vIdx = 0; vIdx < srcVertexCount; vIdx++)
	{
		pMapping[vIdx].ClosestDistance = FLT_MAX; // Want to save smallest, so init to largest possible
		float shortestLength = FLT_MAX;
		float3 vv = pSrcPositions[vIdx];

		// Initialize the barycentrics to known bad value so we can later verify they're set to good value
		pMapping[vIdx].BarycentricCoordinates = float3(-1.0f);

		// Find the closest triangle this vertex projects onto (note: actually project the triangles onto the vertex)
		for (UINT tIdx = 0; tIdx < triangleCount; tIdx++)
		{
			// triangle's vertex indices
			UINT i0 = pTriangleIndices[tIdx * 3 + 0];
			UINT i1 = pTriangleIndices[tIdx * 3 + 1];
			UINT i2 = pTriangleIndices[tIdx * 3 + 2];

			// triangle's vertex positions
			float3 v0 = pTriangleVertexPositions[i0];
			float3 v1 = pTriangleVertexPositions[i1];
			float3 v2 = pTriangleVertexPositions[i2];

			// triangle's normal
			float3 normal = cross3(v1 - v0, v2 - v0);
			float rcpArea = 1.0f / normal.length();
			normal *= rcpArea;

			// Vector from triangle (vertex 0) to vertex
			float3 triangleToVertex = vv - v0;
			float dd = dot3(triangleToVertex, normal); // Closest distance from vertex to plane containing the triangle

			// TODO: Assuming vertex normal follows position.  Assert, etc
			float3 n0 = pTriangleVertexNormals[i0];
			float3 n1 = pTriangleVertexNormals[i1];
			float3 n2 = pTriangleVertexNormals[i2];

			// offsetT# is vertex# offset along it's normal so resulting triangle plane contains the vertex
			float3 offsetT0 = v0 + n0 * dd / dot3(n0, normal);
			float3 offsetT1 = v1 + n1 * dd / dot3(n1, normal);
			float3 offsetT2 = v2 + n2 * dd / dot3(n2, normal);

			// Compute area for projected triangle (well, 2X area. Later divide removes factor of 2) 
			float3 crossOffset = cross3(offsetT1 - offsetT0, offsetT2 - offsetT0);
			float  area = crossOffset.length();
			float  rcpOffsetArea = 1.0f / area;

			// Start computing barycentric coordinates.  Each is 2X area of tri formed by one projected triangle edge and the vertex
			float3 aa = cross3(offsetT2 - offsetT1, vv - offsetT1);
			float3 bb = cross3(offsetT0 - offsetT2, vv - offsetT2);
			float3 cc = cross3(offsetT1 - offsetT0, vv - offsetT0);

            // Compute distance to triangle so we can reject intersections with distant triangles.
            // This addresses the issue that hair can extend beyond the mesh (e.g., below the neck)
            float3 barys = float3( aa.length(), bb.length(), cc.length() ) * rcpOffsetArea;
            float barySum = barys.x + barys.y + barys.z;
            if( barySum <= 1.00001f ) // Note slightly > 1.0 to accomodate floating point errors.
            {
                float3 intersection = barys.x*v0 + barys.y*v1 + barys.z*v2;
                float3 ray = vv - intersection;
                float length = ray.length();
                bool frontFacing = dot3(ray, normal) > 0.0f;
                if( ( (  frontFacing && length < frontFacingDistanceThreshold )
                   || ( !frontFacing && length < backFacingDistanceThreshold ) )
                   && (length < shortestLength) )
                {
                    shortestLength = length;
                    pMapping[vIdx].TriangleIndex = tIdx;           // vertex[vIdx] projects onto triangle[hIdx]
                    pMapping[vIdx].ClosestDistance = length;           // and is dd distance away (along the triangle normal)
                    pMapping[vIdx].BarycentricCoordinates = barys; // and intersects at the point with these barycentric coordinates
                }
            }
		}
	}
}


//-----------------------------------------------------------------------------
void ProjectMeshVerticesOntoMeshTriangles(
    CPUTSoftwareMesh *meshVertices,
    CPUTSoftwareMesh *meshTriangles,
    std::vector<MappedVertex> &outMapped,
    float frontFacingDistanceThreshold,
    float backFacingDistanceThreshold
){
	outMapped.resize(meshVertices->GetVertCount());
	ProjectVerticesOntoTriangles(
		meshVertices->Pos,
		meshVertices->GetVertCount(),
		meshTriangles->IndexBufferCount / 3,
		meshTriangles->IB,
		meshTriangles->Pos,
		meshTriangles->Normal,
		&outMapped[0],
        frontFacingDistanceThreshold,
        backFacingDistanceThreshold
		);
}