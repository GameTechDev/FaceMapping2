/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "CPUTSoftwareMesh.h"
#include "CPUTMeshDX11.h"
#include "CPUTRenderParams.h"

struct SComponentInfo
{
	int Size;
	const char *SenamticName;
	int SemanticIndex;
	DXGI_FORMAT Format;
};
const SComponentInfo gComponentInfo[] =
{
	{ sizeof(float3), "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT }, // eSMComponent_Position,
	{ sizeof(float3), "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT }, // eSMComponent_Normal,
	{ sizeof(float3), "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT }, // eSMComponent_TANGENT,
	{ sizeof(float3), "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT }, // eSMComponent_BiNormal,
	{ sizeof(float2), "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT }, // eSMComponent_Tex1,
	{ sizeof(float2), "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT }  // eSMComponent_Tex2,
};

CPUTSoftwareMesh::CPUTSoftwareMesh() :
Pos(NULL),
Normal(NULL),
Tangent(NULL),
BiNormal(NULL),
Tex(NULL),
Tex2(NULL),
mVertCount(0),
IB(NULL),
MeshTopology(CPUT_TOPOLOGY_INDEXED_TRIANGLE_LIST)
{
	ZeroMemory( mComponentData, sizeof(mComponentData) );
}

CPUTSoftwareMesh::~CPUTSoftwareMesh()
{
	FreeAll();
}

void CPUTSoftwareMesh::FreeAll()
{
	for (int i = 0; i < eSMComponent_Count; i++)
	{
		SAFE_FREE(mComponentData[i]);
		SAFE_FREE(IB);
	}
	IndexBufferCount = 0;
	mVertCount = 0;
}

void CPUTSoftwareMesh::UpdateIndexCount(int count, bool preserveData)
{
	if (IndexBufferCount != count || IB == NULL)
	{
		uint32 *lastIB = IB;
		
		if (count != 0)
		{
			IB = (uint32*)malloc(count * sizeof(uint32));
			if (preserveData && lastIB != NULL)
			{
				memcpy(IB, lastIB, intMin(IndexBufferCount, count) * sizeof(uint32));
			}
		}
		else
		{
			IB = NULL;
		}

		SAFE_FREE(lastIB);
		IndexBufferCount = count;

	}
}

void CPUTSoftwareMesh::UpdateVertexCount(int count, bool preserveData)
{
	if (mVertCount != count)
	{
		for (int i = 0; i < eSMComponent_Count; i++)
		{
			if (mComponentData[i] != NULL)
			{
				void *lastData = mComponentData[i];
				mComponentData[i] = malloc(gComponentInfo[i].Size * count);
				if (preserveData && lastData != NULL)
				{
					memcpy(mComponentData[i], lastData, intMin(count, mVertCount) * gComponentInfo[i].Size);
				}
				SAFE_FREE(lastData);
			}
		}
		mVertCount = count;
		UpdatePointers();
	}
	
}

void CPUTSoftwareMesh::AddComponent(ESMComponent component)
{
	if (mComponentData[component] == NULL)
	{
		SAFE_FREE(mComponentData[component]);
		mComponentData[component] = malloc(gComponentInfo[component].Size * mVertCount);
	}
	UpdatePointers();
}

void CPUTSoftwareMesh::RemoveComponent(ESMComponent component)
{
	SAFE_FREE(mComponentData[component]);
	UpdatePointers();
}

void CPUTSoftwareMesh::UpdatePointers()
{
	// Set up pointers for easy access
	Pos = (float3*)mComponentData[eSMComponent_Position];
	Normal = (float3*)mComponentData[eSMComponent_Normal];
	Tangent = (float3*)mComponentData[eSMComponent_Tangent];
	BiNormal = (float3*)mComponentData[eSMComponent_BiNormal];
	Tex = (float2*)mComponentData[eSMComponent_Tex1];
	Tex2 = (float2*)mComponentData[eSMComponent_Tex2];
}

void CPUTSoftwareMesh::CopyFromDX11Mesh(CPUTMeshDX11 *mesh)
{
	CPUTRenderParameters params = {};
	
	D3D11_MAPPED_SUBRESOURCE mapped = mesh->MapVertices(params, CPUT_MAP_READ);
	
	int srcStride = mesh->GetVertexStride();
	UpdateVertexCount(mesh->GetVertexCount());

	for (D3D11_INPUT_ELEMENT_DESC *layout = mesh->GetLayoutDescription(); layout->SemanticName != NULL; layout++)
	{
		for (int i = 0; i < eSMComponent_Count; i++)
		{
			if (_stricmp(gComponentInfo[i].SenamticName, layout->SemanticName) == 0 &&
				gComponentInfo[i].SemanticIndex == layout->SemanticIndex)
			{
				AddComponent((ESMComponent)i);
				for (int j = 0; j < mVertCount; j++)
				{
					byte *writePos = ((byte*)mComponentData[i]) + gComponentInfo[i].Size * j;
					byte *readPos = ((byte*)mapped.pData) + srcStride * j + layout->AlignedByteOffset;
					memcpy(writePos, readPos, gComponentInfo[i].Size);
				}
			}
		}
	}
	mesh->UnmapVertices(params);

	D3D11_MAPPED_SUBRESOURCE indexMapped = mesh->MapIndices(params, CPUT_MAP_READ, true);
	if (indexMapped.pData)
	{
		int indexCount = mesh->GetIndexCount();
		UpdateIndexCount(indexCount);
		DXGI_FORMAT format = mesh->GetIndexFormat();
		
		if (format == DXGI_FORMAT_R16_UINT)
		{
			uint16 *srcPtr = ((uint16*)indexMapped.pData);
			for (int i = 0; i < indexCount; i++)
			{
				IB[i] = (int)*srcPtr;
				srcPtr++;
			}
		}
		else if (format == DXGI_FORMAT_R32_UINT)
		{
			memcpy(IB, indexMapped.pData, sizeof(uint32) * IndexBufferCount);
		}
		else
		{
			assert(false);
		}
	}
	mesh->UnmapIndices(params);
}

void CPUTSoftwareMesh::CopyFromMultiple(CPUTSoftwareMesh **srcMeshes, int count)
{
	if (count == 0)
	{
		FreeAll();
	}
	else if (count == 1)
	{
		CopyFrom(srcMeshes[0]);
	}
	else
	{
		FreeAll();
		int vertCount = 0;
		int indexCount = 0;
		bool componentInUse[eSMComponent_Count];
		for (int j = 0; j < eSMComponent_Count; j++)
			componentInUse[j] = true;

		for (int i = 0; i < count; i++)
		{
			for (int j = 0; j < eSMComponent_Count; j++)
				componentInUse[j] = componentInUse[j] && srcMeshes[i]->mComponentData[j] != NULL;
			vertCount += srcMeshes[i]->GetVertCount();
			indexCount += srcMeshes[i]->IndexBufferCount;
		}
		UpdateVertexCount(vertCount);
		UpdateIndexCount(indexCount);

		for (int i = 0; i < eSMComponent_Count; i++)
		{
			if (componentInUse[i])
				AddComponent((ESMComponent)i);
		}

		int vertexOffset = 0;
		int indexOffset = 0;
		
		for (int i = 0; i < count; i++)
		{
			CPUTSoftwareMesh *mesh = srcMeshes[i];
			for (int j = 0; j < eSMComponent_Count; j++)
			{
				if (componentInUse[j])
				{
					memcpy((byte*)mComponentData[j] + gComponentInfo[j].Size * vertexOffset, mesh->mComponentData[j], gComponentInfo[j].Size * mesh->mVertCount);
				}
			}
			
			for (int j = 0; j < mesh->IndexBufferCount; j++)
			{
				IB[indexOffset + j] = mesh->IB[j] + vertexOffset;
			}
			vertexOffset += mesh->mVertCount;
			indexOffset += mesh->IndexBufferCount;
		}

	}
}

static int GetBytesFromFormat(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R32G32B32_FLOAT:
		return 12;
	case DXGI_FORMAT_R32G32_FLOAT:
		return 8;
	}
	assert(false);
	return 0;
}

static int CalculateStride(D3D11_INPUT_ELEMENT_DESC *layout)
{
	int size = 0;
	for (D3D11_INPUT_ELEMENT_DESC *layoutIter = layout; layoutIter->SemanticName != NULL; layoutIter++)
	{
		int newSize = layoutIter->AlignedByteOffset + GetBytesFromFormat(layoutIter->Format);
		size = newSize > size ? newSize : size;
	}
	return size;
}

void CPUTSoftwareMesh::CopyVertexDataToBuffer(D3D11_INPUT_ELEMENT_DESC *layout, void *buffer, int bufferSize)
{
	int stride = CalculateStride(layout);
	for (D3D11_INPUT_ELEMENT_DESC *layoutIter = layout; layoutIter->SemanticName != NULL; layoutIter++)
	{
		// find the corresponding component
		for (int i = 0; i < eSMComponent_Count; i++)
		{
			if (gComponentInfo[i].SemanticIndex == layoutIter->SemanticIndex  && !_stricmp(gComponentInfo[i].SenamticName, layoutIter->SemanticName))
			{
				assert(gComponentInfo[i].Format == layoutIter->Format);
				assert((mVertCount * stride) <= bufferSize);
				if (gComponentInfo[i].Format == DXGI_FORMAT_R32G32_FLOAT)
				{
					float2 *srcData = (float2 *)mComponentData[i];
					for (int j = 0; j < mVertCount; j++)
					{
						float2* dst = (float2*)&((byte*)buffer)[j * stride + layoutIter->AlignedByteOffset];
						*dst = srcData[j];
					}
				}
				else if(gComponentInfo[i].Format == DXGI_FORMAT_R32G32B32_FLOAT)
				{
					float3 *srcData = (float3 *)mComponentData[i];
					for (int j = 0; j < mVertCount; j++)
					{
						float3* dst = (float3*)&((byte*)buffer)[j * stride + layoutIter->AlignedByteOffset];
						*dst = srcData[j];
					}
				}
				else
				{
					assert(false);
				}
				break;
			}
		}
	}
}

int CPUTSoftwareMesh::BuildDXVertLayout(D3D11_INPUT_ELEMENT_DESC *layout, int layoutCount)
{
	ZeroMemory(layout, sizeof(D3D11_INPUT_ELEMENT_DESC) * layoutCount);
	int count = 0;
	int offset = 0;
	for (int i = 0; i < eSMComponent_Count; i++)
	{
		if (mComponentData[i] != NULL)
		{
			assert(count < (layoutCount - 1));
			if (count < (layoutCount - 1))
			{
				layout[count].Format = gComponentInfo[i].Format;
				layout[count].AlignedByteOffset = offset;
				layout[count].SemanticName = gComponentInfo[i].SenamticName;
				layout[count].SemanticIndex = gComponentInfo[i].SemanticIndex;
				offset += gComponentInfo[i].Size;
				count++;
			}
		}
	}
	return count;
}

void CPUTSoftwareMesh::CopyToDX11Mesh(CPUTMeshDX11 *dstMesh)
{
	D3D11_INPUT_ELEMENT_DESC layout[eSMComponent_Count + 1];
	int count = BuildDXVertLayout(layout, ARRAYSIZE(layout));
	int stride = CalculateStride(layout);
	
	dstMesh->UpdateVertCountAndFormat(mVertCount, layout, stride);

	CPUTRenderParameters params = {};
	D3D11_MAPPED_SUBRESOURCE mapped = dstMesh->MapVertices(params, CPUT_MAP_WRITE);

	CopyVertexDataToBuffer(layout, mapped.pData, dstMesh->GetVertexCount() * dstMesh->GetVertexStride());

	dstMesh->UnmapVertices(params);

	dstMesh->SetIndexBufferData(IB, IndexBufferCount);
	dstMesh->SetMeshTopology(MeshTopology);
}

void CPUTSoftwareMesh::ApplyTransform(float4x4 *transform)
{
	if (transform)
	{
		float4x4 matrix = *transform;
		if (this->Pos)
		{
			for (int i = 0; i < mVertCount; i++)
			{
				float4 pos4 = float4(Pos[i], 1.0f) * matrix;
				Pos[i] = float3(pos4.x / pos4.w, pos4.y / pos4.w, pos4.z / pos4.w);
			}
		}

		float4x4 rotMatrix = *transform;
		rotMatrix.r0.w = 0.0f;
		rotMatrix.r1.w = 0.0f;
		rotMatrix.r2.w = 0.0f;

		if (this->Normal)
		{
			for (int i = 0; i < mVertCount; i++)
			{
				float4 norm = float4(Normal[i].x, Normal[i].y, Normal[i].z, 1.0f);
				norm = norm * rotMatrix;
				Normal[i] = float3(norm.x, norm.y, norm.z);
				Normal[i].normalize();
			}
		}
		if (this->Tangent)
		{
			for (int i = 0; i < mVertCount; i++)
			{
				float4 norm = float4(Tangent[i].x, Tangent[i].y, Tangent[i].z, 1.0f);
				norm = norm * rotMatrix;
				Tangent[i] = float3(norm.x, norm.y, norm.z);
				Tangent[i].normalize();
			}
		}
		if (this->BiNormal)
		{
			for (int i = 0; i < mVertCount; i++)
			{
				float4 norm = float4(BiNormal[i].x, BiNormal[i].y, BiNormal[i].z, 1.0f);
				norm = norm * rotMatrix;
				BiNormal[i] = float3(norm.x, norm.y, norm.z);
				BiNormal[i].normalize();
			}
		}
	}
}

void CPUTSoftwareMesh::CopyFrom(CPUTSoftwareMesh *srcMesh)
{
	
	for (int i = 0; i < eSMComponent_Count; i++)
	{

		if (srcMesh->mComponentData[i] == NULL) // remove components that don't exist in source
		{
			SAFE_FREE(mComponentData[i]);
		}
		else
		{
			int size = gComponentInfo[i].Size * srcMesh->mVertCount;
			if (mVertCount != srcMesh->mVertCount || mComponentData[i] == NULL)
			{
				mComponentData[i] = malloc(size);
			}
			memcpy(mComponentData[i], srcMesh->mComponentData[i], size);
		}
	}
	UpdatePointers();

	MeshTopology = srcMesh->MeshTopology;
	mVertCount = srcMesh->mVertCount;

	// Index Buffer
	UpdateIndexCount(srcMesh->IB != NULL ? srcMesh->IndexBufferCount : 0);
	memcpy(IB, srcMesh->IB, sizeof(uint32) * srcMesh->IndexBufferCount);

}

bool CPUTSoftwareMesh::Intersect(float3 point, float3 dir, float3 *outPos)
{
	int triCount = IndexBufferCount / 3;
	for (int i = 0; i < triCount; i++)
	{
		float3 &pos1 = Pos[IB[i * 3]];
		float3 &pos2 = Pos[IB[i * 3 + 1]];
		float3 &pos3 = Pos[IB[i * 3 + 2]];
	}
	return true;
}