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
#include "CPUT.h"
#include "CHeadGeometryStage.h"
#include "CDisplacementMapStage.h"
#include "CPUTMeshDX11.h"
#include "CPUTTextureDX11.h"
#include "CPUTModel.h"
#include "CPUTSoftwareTexture.h"
#include "CPipeline.h"
#include "CPUTSoftwareMesh.h"

struct HeadVertex
{
	float3 Position;
	float3 Normal;
	float2 UV1; // head texture coords
	float2 UV2; // face map texture coords
};

static D3D11_INPUT_ELEMENT_DESC gHeadVertexDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ NULL, 0, (DXGI_FORMAT)0, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

struct HeadProjectionInfo
{
	// Converts head vertex position in model space to texture coordinants on generated maps
	float4x4 ViewProjTexMatrix;

	// Converts from the map coordinates to model space
	float4x4 MapToHeadSpaceTransform;

	// max/min z displacement in head model space. The maximum displacement should be applied when the depth
	float ExtrudeMinZ;
	float ExtrudeMaxZ;

	// used to convert the depth value from the depth map into 
	float DepthMapRangeMin;
	float DepthMapRangeMax;
};


CHeadGeometryStage::CHeadGeometryStage()
{
	mMapProjectionCamera = CPUTCamera::Create(CPUT_ORTHOGRAPHIC);
}

CHeadGeometryStage::~CHeadGeometryStage()
{
	SAFE_RELEASE(mMapProjectionCamera);
}


void CHeadGeometryStage::UpdateHeadProjectionInfo(CDisplacementMapStageOutput *dispMapInfo, SBaseHeadInfo *headInfo, MappingTweaks *tweaks, HeadProjectionInfo *outProjInfo)
{
	float eyeDistanceMS; // model space
	float2 mapScaleFactor;
	float zScaleMSToRS;

	float3 leftEyeCenter = headInfo->BaseHeadLandmarks[kLandmarkIndex_LeftEyeAnchor];
	float3 rightEyeCenter = headInfo->BaseHeadLandmarks[kLandmarkIndex_RightEyeAnchor];
	float3 noseTip = headInfo->BaseHeadLandmarks[kLandmarkIndex_NoseTip];

	// calculate head model scale metric
	eyeDistanceMS = abs(leftEyeCenter.x - rightEyeCenter.x);
	float3 anchor = (leftEyeCenter + rightEyeCenter) / 2.0f;
	float2 anchorMS = float2(anchor.x, anchor.y);

	// calculate scaling
	mapScaleFactor.x = (eyeDistanceMS / dispMapInfo->EyeDistance_MapSpace) *tweaks->Scale;
	mapScaleFactor.y = (eyeDistanceMS / dispMapInfo->EyeDistance_MapSpace) *tweaks->Scale;
	zScaleMSToRS = (eyeDistanceMS / dispMapInfo->EyeDistance_FaceModelSpace) *tweaks->Scale;
	float zScaleRSToMS = 1.0f / zScaleMSToRS;

	// calculate the projection onto the head model
	float2 headProjectionPos;
	headProjectionPos.y = anchorMS.y + (dispMapInfo->Anchor_MapSpace.y * mapScaleFactor.y);
	headProjectionPos.x = anchorMS.x - (dispMapInfo->Anchor_MapSpace.x * mapScaleFactor.x);

	float y = 5.0f;
	mMapProjectionCamera->SetAspectRatio(1.0f);
	mMapProjectionCamera->SetWidth(2.0f * mapScaleFactor.x);
	mMapProjectionCamera->SetHeight(2.0f * mapScaleFactor.y);
	mMapProjectionCamera->SetPosition(headProjectionPos.x, headProjectionPos.y, -10.0f);
	mMapProjectionCamera->LookAt(headProjectionPos.x, headProjectionPos.y, 0.0f);
	mMapProjectionCamera->Update();
	float4x4 viewMatrix = *mMapProjectionCamera->GetViewMatrix();
	float4x4 projMatrix = *mMapProjectionCamera->GetProjectionMatrix();
	float4x4 deviceToTex = float4x4Scale(0.5f, -0.5f, 0.0f) * float4x4Translation(0.5f,0.5f,0.0f); // device normal coordinates to texture coords
	outProjInfo->ViewProjTexMatrix = viewMatrix * projMatrix * deviceToTex;

	// 5 units in model space. This is a hardcoded offset from the tip of the nose. This could be specified with a model landmark
	float extrudeMaxZ = 7;
	float headMSUnitsPerRSUnitsZNormalized = zScaleRSToMS / dispMapInfo->DepthMap_ZRange;
	float extrudeBaseZ = noseTip.z + extrudeMaxZ;

	// max is nose tip
	outProjInfo->ExtrudeMinZ = noseTip.z + extrudeMaxZ + tweaks->DisplaceOffset.z;
	outProjInfo->ExtrudeMaxZ = outProjInfo->ExtrudeMinZ - extrudeMaxZ* tweaks->Scale;
	outProjInfo->DepthMapRangeMin = dispMapInfo->DepthMap_ZMeshStart - extrudeMaxZ * headMSUnitsPerRSUnitsZNormalized;
	outProjInfo->DepthMapRangeMax = dispMapInfo->DepthMap_ZMeshStart;

	
	float4x4 mapToHead = float4x4Scale( mapScaleFactor.x, -mapScaleFactor.y, 1.0f);
	mapToHead = mapToHead * float4x4Translation(headProjectionPos.x, headProjectionPos.y, 0.0f);
	outProjInfo->MapToHeadSpaceTransform = mapToHead;
}





inline float RemapRange(float value, float r1Min, float r1Max, float r2Min, float r2Max)
{
	float ratio = (value - r1Min) / (r1Max - r1Min);
	ratio = floatClamp(ratio, 0.0f, 1.0f);
	return r2Min + ratio * (r2Max - r2Min);
}

static void ApplyMorphTargets(std::vector<MorphTargetEntry> &entries, CPUTSoftwareMesh *mesh, bool post)
{
	// Apply all the morph targets
	int vertCount = mesh->GetVertCount();
	for (auto it = entries.begin(); it != entries.end(); ++it) {
		if (it->Post == post)
		{
			CMorphTarget *morphTarget = it->Target;
			for (auto vit = morphTarget->MorphVerts.begin(); vit != morphTarget->MorphVerts.end(); ++vit)
			{
				assert(vit->VertIndex < vertCount);
				mesh->Pos[vit->VertIndex] += vit->Delta * it->Weight;
				mesh->Normal[vit->VertIndex] += vit->NormalDelta * it->Weight;
			}
		}
	}

	for (int i = 0; i < vertCount; i++)
		mesh->Normal[i].normalize();
}

void CalculateMeshNormals(float3 *positions, uint32 *faces, int vertexCount, int faceCount, float3 *outNormals)
{
	ZeroMemory(outNormals, sizeof(float3) *vertexCount);
	for (int i = 0; i < faceCount; i++)
	{
		uint32 idx1 = faces[i * 3];
		uint32 idx2 = faces[i * 3 + 1];
		uint32 idx3 = faces[i * 3 + 2];

		float3 posA = positions[idx1];
		float3 posB = positions[idx2];
		float3 posC = positions[idx3];

		float3 vecAtoB = posB - posA;
		float3 vecBtoC = posC - posB;

		float3 normal = cross3(vecAtoB, vecBtoC).normalize();
		
		outNormals[idx1] += normal;
		outNormals[idx2] += normal;
		outNormals[idx3] += normal;
	}

	for (int i = 0; i < vertexCount; i++)
	{
		outNormals[i] = outNormals[i].normalize();
	}
}

void CHeadGeometryStage::Execute(SHeadGeometryStageInput *input)
{
	CPUTSoftwareMesh *base = input->BaseHeadInfo->BaseHeadMesh;
	
	// Find the vertex index that each head landmark belongs to
	std::vector<float3> MorphedHeadLandmarks;
	
	// First, correlate landmark mesh vertices to landmarks
	// Potentially difficult to force vertex indices from authoring package (e.g., 3dsMax).
	// Easier to force order of assets in asset set (note, 3dsMax appears to use selection order).
	// At worst, can manually edit asset.set file.
	// So, search landmark asset set for landmark at position of each vertex
	
	LandmarkMeshVertexToLandmarkIndex.clear();
	CPUTSoftwareMesh *headLMMesh = &input->BaseHeadInfo->LandmarkMesh;
	std::vector<float3> &headLM = input->BaseHeadInfo->BaseHeadLandmarks;
	int landmarkCount = (int)headLM.size();
	int lmVertCount = headLMMesh->GetVertCount();
	for (int i = 0; i < lmVertCount; i++)
	{
		LandmarkMeshVertexToLandmarkIndex.push_back(-1);
		float closestDistance = FLT_MAX;
		for (int j = 0; j < landmarkCount; j++)
		{
			float3 vertexToLandmark = headLMMesh->Pos[i] - headLM[j];
			vertexToLandmark.z = 0.0f; // Ignore depth (i.e., project landmark onto landmark mesh plane)
			float distance = abs(vertexToLandmark.length());
			if (distance < 1.0f && distance < closestDistance)
			{
				closestDistance = distance;
				LandmarkMeshVertexToLandmarkIndex[i] = j;
			}
		}
	}

	HeadProjectionInfo hpi;
	UpdateHeadProjectionInfo(input->DisplacementMapInfo, input->BaseHeadInfo, input->Tweaks, &hpi);

	MorphedLandmarkMesh.CopyFrom(&input->BaseHeadInfo->LandmarkMesh);
		
	// Shift the landmark mesh to match the face landmarks
	for (int i = 0; i < lmVertCount; i++)
	{
		int idx = LandmarkMeshVertexToLandmarkIndex[i];
		if (idx != -1)
		{
			float2 lmMapPos = input->DisplacementMapInfo->MapLandmarks[idx];
			float4 pos = float4(lmMapPos.x, lmMapPos.y, MorphedLandmarkMesh.Pos[i].z, 1.0f);
			MorphedLandmarkMesh.Pos[i] = pos * hpi.MapToHeadSpaceTransform;
		}
	}

	if (mMappedFaceVertices.size() != input->BaseHeadInfo->BaseHeadMesh->GetVertCount() || input->ClearCachedProjections)
	{
		// Project face vertices onto original landmark mesh
		ProjectMeshVerticesOntoMeshTriangles(input->BaseHeadInfo->BaseHeadMesh, &input->BaseHeadInfo->LandmarkMesh, mMappedFaceVertices);
	}
	
	CPUTSoftwareMesh *dstMesh = &DeformedMesh;
	dstMesh->CopyFrom(input->BaseHeadInfo->BaseHeadMesh);

	assert(base->Pos);
	assert(base->Tex);

	int vertCount = base->GetVertCount();

	// Apply all the morph targets
	ApplyMorphTargets(input->Tweaks->MorphTargetEntries, &DeformedMesh, false);
	

	dstMesh->AddComponent(eSMComponent_Tex2);

	
	CPUTSoftwareTexture *controlMapColor = ((CPUTTextureDX11*)input->BaseHeadInfo->Textures[eBaseHeadTexture_ControlMap_Color])->GetSoftwareTexture(false, true);

	if ((input->Tweaks->Flags & PIPELINE_FLAG_SkipFitFace) == 0)
	{
		assert(vertCount == mMappedFaceVertices.size());
		for (int vIdx = 0; vIdx < vertCount; vIdx++)
		{
			float dd = mMappedFaceVertices[vIdx].ClosestDistance;
			if (dd != FLT_MAX)
			{
				int lIdx = mMappedFaceVertices[vIdx].TriangleIndex;   // headVertex[hIdx] projects onto landmarkMeshTriangle[lIdx]
				assert(lIdx < MorphedLandmarkMesh.IndexBufferCount);
				UINT i0 = MorphedLandmarkMesh.IB[lIdx * 3 + 0];
				UINT i1 = MorphedLandmarkMesh.IB[lIdx * 3 + 1];
				UINT i2 = MorphedLandmarkMesh.IB[lIdx * 3 + 2];

				float3 v0 = MorphedLandmarkMesh.Pos[i0];
				float3 v1 = MorphedLandmarkMesh.Pos[i1];
				float3 v2 = MorphedLandmarkMesh.Pos[i2];

				float3 barys = mMappedFaceVertices[vIdx].BarycentricCoordinates;
				float3 newPos = v0*barys.x + v1*barys.y + v2*barys.z;
				newPos.z += dd;

				//newPos = float4(newPos, 1.0f);// *invWorld;
				CPUTColor4 samp(0.0f, 0.0f, 0.0f, 0.0f);
				controlMapColor->SampleRGBAFromUV(dstMesh->Tex[vIdx].x, dstMesh->Tex[vIdx].y, &samp);
				
				dstMesh->Pos[vIdx] = dstMesh->Pos[vIdx] * (1.0f - samp.r) + newPos * samp.r;
				
			}
		}
	}

	CPUTSoftwareTexture *controlMapDisplacement = ((CPUTTextureDX11*)input->BaseHeadInfo->Textures[eBaseHeadTexture_ControlMap_Displacement])->GetSoftwareTexture(false, true);

	CPUTColor4 dispSample(0.0f, 0.0f, 0.0f, 0.0f);
	CPUTColor4 controlSample(0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = 0; i < vertCount; i++)
	{
		float3 pos = dstMesh->Pos[i];

		// project displacement map and color map
		float3 projectedUV = float4(pos, 1.0f) * hpi.ViewProjTexMatrix;
		
		input->DisplacementMap->SampleRGBAFromUV(projectedUV.x, projectedUV.y, &dispSample);

		float displacedZ = RemapRange(dispSample.r, hpi.DepthMapRangeMin, hpi.DepthMapRangeMax, hpi.ExtrudeMinZ, hpi.ExtrudeMaxZ);

		controlMapDisplacement->SampleRGBAFromUV(dstMesh->Tex[i].x, dstMesh->Tex[i].y, &controlSample);

		// Blend between displaced Z and default model z based on the control texture's red value
		if ((input->Tweaks->Flags & PIPELINE_FLAG_SkipDisplacmentMap) == 0)
			pos.z = floatLerp(pos.z, displacedZ, controlSample.r);

		dstMesh->Tex2[i] = float2(projectedUV.x, projectedUV.y);
		dstMesh->Pos[i] = pos;
	}

	
	// Apply all the morph targets
	ApplyMorphTargets(input->Tweaks->MorphTargetEntries, &DeformedMesh, true);

	if (input->Tweaks->OtherHeadBlend > 0.0f && input->Tweaks->OtherHeadMesh != NULL)
	{
		for (int i = 0; i < vertCount; i++)
		{
			dstMesh->Pos[i] = dstMesh->Pos[i] + (input->Tweaks->OtherHeadMesh->Pos[i] - dstMesh->Pos[i]) * input->Tweaks->OtherHeadBlend;
		}
	}

	// Fix up normals now that we have imprinted the face
	float3 *tempNormals = (float3*)malloc(sizeof(float3) * vertCount);
	CalculateMeshNormals(dstMesh->Pos, dstMesh->IB, vertCount, dstMesh->IndexBufferCount / 3, tempNormals);
	for (int i = 0; i < vertCount; i++)
	{
		controlMapDisplacement->SampleRGBAFromUV(dstMesh->Tex[i].x, dstMesh->Tex[i].y, &controlSample);
		dstMesh->Normal[i] = tempNormals[i] * controlSample.r + dstMesh->Normal[i] * (1.0f - controlSample.r);
		dstMesh->Normal[i] = dstMesh->Normal[i].normalize();
	}
	free(tempNormals);

}

