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
#ifndef __FM_PIPELINE_STATE__
#define __FM_PIPELINE_STATE__

#include "CPUTSoftwareMesh.h"

#include <vector>

class CPUTModel;
class CDisplacementMapStage;
class CHeadGeometryStage;
class CHeadBlendStage;
class CPUTRenderParameters;
class CFaceModel;
class CPUTSoftwareMesh;
class CHairGeometryStage;
class MappingTweaks;

const int kMaxHeadBlends = 4;

enum PIPELINE_FLAG
{
	// Debug Flags
	PIPELINE_FLAG_SkipFitFace = (1 << 16),
	PIPELINE_FLAG_SkipDisplacmentMap = (1 << 17),
	PIPELINE_FLAG_SkipFaceColorBlend = (1 << 18),
	PIPELINE_FLAG_SkipColorSeamFill = (1 << 19),
};

const int kLandmarkIndex_LeftEye = 76;
const int kLandmarkIndex_RightEye = 77;
const int kLandmarkIndex_LeftEyeOutside = 14;
const int kLandmarkIndex_RightEyeOutside = 22;
const int kLandmarkIndex_NoseTip = 30;
const int kLandmarkIndex_FaceOutlineStart = 53;
const int kLandmarkIndex_FaceOutlineEnd = 69;

const int kLandmarkIndex_LeftEyeAnchor = kLandmarkIndex_LeftEyeOutside;
const int kLandmarkIndex_RightEyeAnchor = kLandmarkIndex_RightEyeOutside;

enum EBaseHeadTexture
{
	eBaseHeadTexture_ControlMap_Displacement,
	eBaseHeadTexture_ControlMap_Color,
	eBaseHeadTexture_FeatureMap,
	eBaseHeadTexture_ColorTransfer,
	eBaseHeadTexture_Skin,
	eBaseHeadTexture_Count
};

struct SBaseHeadInfo
{
	std::string Name;
	
	std::vector<float3> BaseHeadLandmarks;

	CPUTSoftwareMesh LandmarkMesh;

	CPUTSoftwareMesh *BaseHeadMesh;
	CPUTTexture *Textures[eBaseHeadTexture_Count];
};


struct MorphTargetVertex
{
	int VertIndex;
	float3 Delta;
	float3 NormalDelta;
};

class CMorphTarget
{
public:
	std::vector<MorphTargetVertex> MorphVerts;
};

struct MorphTargetEntry
{
	CMorphTarget *Target;
	float Weight;
	bool Post;
	bool operator==(const MorphTargetEntry &other) const { return Target == other.Target && Weight == other.Weight; }
};

enum PostBlendColorMode
{
	PostBlendColorMode_None,
	PostBlendColorMode_Colorize,
	PostBlendColorMode_Adjust,
	PostBlendColorMode_Count
};

// User tweakable variables. Will try to create 
class MappingTweaks
{
public:
	float Scale;

	// Displacement applied in head space
	float3 DisplaceOffset;

	// Rotation applied to the RealSense model before creating the face maps
	float FaceYaw;
	float FacePitch;
	float FaceRoll;

	CPUTColor4 BlendColor1;
	CPUTColor4 BlendColor2;

	PostBlendColorMode PostBlendMode;
	int3 PostBlendColorize[2];
	int3 PostBlendAdjust[2];

	int OutputTextureResolution;

	std::vector<MorphTargetEntry> MorphTargetEntries;

	int Flags;

	float OtherHeadBlend;
	CPUTSoftwareMesh *OtherHeadMesh;
	CPUTTexture *OtherHeadTexture;

	bool operator==(const MappingTweaks &other) const;
	bool operator!=(const MappingTweaks &other) const { return !(*this == other); }
};

struct SHairPipelineInfo
{
	SHairPipelineInfo(CPUTSoftwareMesh *mesh) { Mesh = mesh; }
	SHairPipelineInfo() { Mesh = NULL; }
	CPUTSoftwareMesh *Mesh;
};

struct SPipelineInput
{
	CPUTRenderParameters *RenderParams;
	CFaceModel *FaceModel;
	
	SBaseHeadInfo *BaseHeadInfo;
	MappingTweaks *Tweaks;

	std::vector<SHairPipelineInfo> HairInfo;
};

class CPipelineOutput
{
public:
	CPipelineOutput() : DeformedMesh(NULL), DiffuseTexture(NULL){}

	CPUTSoftwareMesh *DeformedMesh;
	CPUTTexture *DiffuseTexture;

	std::vector<CPUTSoftwareMesh*> DeformedHairMeshes;
};

class CPipeline
{
public:
	CPipeline();
	~CPipeline();

	void Execute(SPipelineInput *input, CPipelineOutput *output);

	void GetOutput(CPipelineOutput *output);

	CDisplacementMapStage *DisplacementMapStage;
	CHeadGeometryStage *HeadGeometryStage;
	CHeadBlendStage *HeadBlendStage;

	std::vector<CHairGeometryStage*> HairStages;

private:
	CPUTSoftwareMesh mDeformedMesh;
};

#endif