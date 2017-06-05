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
#ifndef _CPUTSOFTWAREMESH_H
#define _CPUTSOFTWAREMESH_H

#include <d3d11.h>
#include "CPUT.h"
#include "CPUTMesh.h"

enum ESMComponent
{
	eSMComponent_Position,
	eSMComponent_Normal,
	eSMComponent_Tangent,
	eSMComponent_BiNormal,
	eSMComponent_Tex1,
	eSMComponent_Tex2,
	eSMComponent_Count
};

class CPUTMeshDX11;

class CPUTSoftwareMesh
{
public:
	CPUTSoftwareMesh();
	~CPUTSoftwareMesh();
	
	void UpdateVertexCount(int count, bool preserveData = false);
	void UpdateIndexCount(int count, bool preserveData = false);

	void AddComponent(ESMComponent component);
	void RemoveComponent(ESMComponent component);

	void CopyFromDX11Mesh(CPUTMeshDX11 *mesh);
	void CopyFrom(CPUTSoftwareMesh *srcMesh);
	void CopyFromMultiple(CPUTSoftwareMesh **srcMeshes, int count);

	void ApplyTransform(float4x4 *transform);

	void CopyToDX11Mesh(CPUTMeshDX11 *dstMesh);

	int GetVertCount() { return mVertCount; }

	float3 *Pos;
	float3 *Normal;
	float3 *Tangent;
	float3 *BiNormal;
	float2 *Tex;
	float2 *Tex2;

	uint32 *IB;
	int IndexBufferCount;

	eCPUT_MESH_TOPOLOGY MeshTopology;

	void FreeAll();

	bool Intersect(float3 point, float3 dir, float3 *outPos);

private:

	void CopyVertexDataToBuffer(D3D11_INPUT_ELEMENT_DESC *layout, void *buffer, int bufferSize);
	int BuildDXVertLayout(D3D11_INPUT_ELEMENT_DESC *layout, int layoutCount);
	
	void *mComponentData[eSMComponent_Count];
	int mVertCount;

	void UpdatePointers();
};

#endif //_CPUTSOFTWARETEXTURE_H