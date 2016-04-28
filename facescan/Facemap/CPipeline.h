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