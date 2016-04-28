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