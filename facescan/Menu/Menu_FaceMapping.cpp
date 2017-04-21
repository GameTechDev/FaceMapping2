/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "Menu_FaceMapping.h"
#include "MenuController.h"
#include "CPUTGuiControllerDX11.h"
#include "MenuGlob.h"
#include "CPUTModel.h"
#include "CPUTMaterial.h"
#include "CPUTSprite.h"
#include "CPUTScene.h"
#include "CPUT_DX11.h"
#include "../SampleStart.h"
#include <string>
#include "CPUTVertexShaderDX11.h"
#include "CPUTGeometryShaderDX11.h"
#include "CPUTSoftwareMesh.h"
#include "..\FaceMap\FaceMapUtil.h"
#include "..\facemap\CDisplacementMapStage.h"
#include "..\facemap\CHeadGeometryStage.h"
#include "..\facemap\CHeadBlendStage.h"
#include "..\OBJExporter.h"

#include "../imgui/imgui_impl_dx11.h"

#include <DXGItype.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <DXProgrammableCapture.h>
const char *sDebugTextureViewNames[] = 
{
	"None",
	"Displacement Color",
	"Displacement Depth",
	"Final Head Diffuse"
};
static int cAssert = 1 / ((ARRAYSIZE(sDebugTextureViewNames) == DebugTextureView_Count) ? 1 : 0);

const char *sDebugHeadDisplayTextureNames[] =
{
	"None",
	"Displacement Control Map",
	"Color Control Map",
	"Feature Map",
	"Color Transfer Map",
	"Skin Map"
};
static int cAssert2 = 1 / ((ARRAYSIZE(sDebugHeadDisplayTextureNames) == DebugHeadDisplayTexture_Count) ? 1 : 0);

const char *sPostBlendColorModes[] =
{
	"None",
	"Colorize",
	"Adjust",
};
static int cAssert3 = 3 / ((ARRAYSIZE(sPostBlendColorModes) == PostBlendColorMode_Count) ? 1 : 0);

const char *sCameraModes[] =
{
	"Model Viewer",
	"Free",
	"Orthographic"
};
static int cAssert4 = 3 / ((ARRAYSIZE(sCameraModes) == CameraMode_Count) ? 1 : 0);

void SetDefaultTweaks(MappingTweaks *tweaks)
{
	tweaks->Scale = 1.0f;
	tweaks->FaceYaw = 0.0f;
	tweaks->FacePitch = 0.0f;
	tweaks->FaceRoll = 0.0f;
	tweaks->OutputTextureResolution = 2048;
	tweaks->DisplaceOffset = float3(0.0f, 0.0f, -3.5f);
	tweaks->BlendColor1 = CPUTColorFromBytes(228, 194, 171, 255);
	tweaks->BlendColor2 = CPUTColorFromBytes(205, 50, 50, 255);
	tweaks->PostBlendAdjust[0] = tweaks->PostBlendAdjust[1] = int3(0, 0, 0);
	tweaks->PostBlendColorize[0] = tweaks->PostBlendColorize[1] = int3(180, 50, 0);
	tweaks->PostBlendMode = PostBlendColorMode_None;
	tweaks->OtherHeadBlend = 0.0f;
	tweaks->OtherHeadTexture = NULL;
	tweaks->OtherHeadMesh = NULL;
}

static CPUTTexture *LoadTexture(std::string &dir, const char *filename)
{
	std::string textureName;
	CPUTFileSystem::CombinePath(dir, filename, &textureName);
	return CPUTTexture::Create(std::string("dynamicLoad"), textureName, false);
}

static bool LoadCPUTModelToSWMesh(CPUTAssetSet *set, const char *modelName, CPUTSoftwareMesh *outMesh)
{
	CPUTModel *model = NULL;
	CPUTResult result = set->GetAssetByName(modelName, (CPUTRenderNode**)&model);
	assert(result == CPUT_SUCCESS);

	// get first mesh
	CPUTMeshDX11 *dx11Mesh = (CPUTMeshDX11 *)model->GetMesh(0);
	outMesh->CopyFromDX11Mesh(dx11Mesh);
	outMesh->ApplyTransform(model->GetWorldMatrix());
	SAFE_RELEASE(model);
	return true;
}

void Menu_FaceMapping::SetDefaultDebug()
{
	mDebugTextureView = DebugTextureView_None;
	mDebugHeadDisplayTextureView = DebugHeadDisplayTexture_None;
	mShowMapLandmarks = true;
	mRenderLandmarkMesh = false;
	mRenderMorphedLandmarkMesh = false;
	mRenderHeadLandmarks = false;
	mShowWireframe = false;
	mFullscreenDebugTextureViewer = true;
	mUseOrthoCamera = false;
	mSkipFaceFit = false;
	mHideCubeMap = false;
	mSkipFaceDisplace = false;
	mSkipFaceColorBlend = false;
	mSkipSeamFill = false;


	

	mDirectionalLightHeight = 0.0f;
	mDirectionalLightAngle = 0.0f;
	mDirectionalLightIntensity = 0.7f;
	mAmbientLightIntensity = 0.3f;

}

void Menu_FaceMapping::ResetCameraDefaults()
{
	CPUTCamera *fpsCamera = CPUTCamera::Create(CPUT_PERSPECTIVE);
	mCameraControlFPS->SetCamera(fpsCamera);
	mCameraControlFPS->SetLookSpeed(0.005f);
	mCameraControlFPS->SetMoveSpeed(50.0f);
	fpsCamera->SetPosition(float3(0.0f, 0.0f, -50.0f));
	fpsCamera->SetFov(33.0f * float(3.14159265359 / 180.0));
	fpsCamera->SetNearPlaneDistance(0.1f);
	fpsCamera->SetFarPlaneDistance(400.0f);
	SAFE_RELEASE(fpsCamera);

	CPUTCamera *pCamera = (CPUTCamera*)mCameraControlViewer->GetCamera();
	pCamera->SetFov(33.0f * float(3.14159265359 / 180.0));
	pCamera->SetNearPlaneDistance(0.1f);
	pCamera->SetFarPlaneDistance(400.0f);

	mCameraControlViewer->SetTarget(float3(0, 0, 0));
	mCameraControlViewer->SetDistance(80.0f, 0.1f, 400.0f);
	mCameraControlViewer->SetViewAngles(0, 0);
	
	mCameraControlOrthographic->SetPosition(float3(0.0f, 0.0f, -50.0f));
	mCameraControlOrthographic->SetLook(float3(0, 0, 0));
	mCameraControlOrthographic->SetZoomRange(1.0f, 1.0f, 40.0f, 40.0f);
	mCameraControlOrthographic->SetVolume(float3(-20.0f, -20.0f, -100.0f), float3(20.0f, 20.0f, -100.0f));
}

void Menu_FaceMapping::Init()
{
	MenuBase::Init();

	mCPUTLandmarkModel = NULL;
	
	mForceRebuildAll = true;
	mIsEditingLandmarks = false;
	mOtherHeadTexture = NULL;
		
	SetDefaultDebug();
	
	SetDefaultTweaksInternal();
	mLastTweaks.Scale = 0.0f;

	CPUTAssetLibrary *pAssetLibrary = CPUTAssetLibrary::GetAssetLibrary();
	
    {
        std::string mediaDir;
        CPUTFileSystem::GetMediaDirectory(&mediaDir);
        std::string skyDir = mediaDir + "/cubeMap_01/";
        CPUTAssetLibrary* pAssetLibrary = CPUTAssetLibrary::GetAssetLibrary();
        pAssetLibrary->SetMediaDirectoryName(skyDir);
        mpCubemap = pAssetLibrary->GetAssetSet("cubeMap_01");
        pAssetLibrary->SetMediaDirectoryName(mediaDir + "/");
    }
	pAssetLibrary->SetRootRelativeMediaDirectory("MyAssets");
	CPUTMaterial* mat = pAssetLibrary->GetMaterial("render_code_sprite");
	mDebugTextureSprite = CPUTSprite::Create(-1.0f, -1.0f, 0.5f, 0.5f, mat);
	SAFE_RELEASE(mat);

	// load the head model
	std::string mediaFilename;
	CPUTFileSystem::GetMediaDirectory(&mediaFilename);
	pAssetLibrary->SetMediaDirectoryName(mediaFilename+"\\");
	CPUTFileSystem::CombinePath(mediaFilename, "headassets.scene", &mediaFilename);
	mHeadAssetScene = CPUTScene::Create();
	mHeadAssetScene->LoadScene(mediaFilename);
	
	CPUTAssetSet *landmarkSet = mHeadAssetScene->GetAssetSet(0);
	CPUTAssetSet *landmarkMeshSet = mHeadAssetScene->GetAssetSet(1);
	CPUTAssetSet *headSet = mHeadAssetScene->GetAssetSet(2);
	CPUTAssetSet *hairShortSet = mHeadAssetScene->GetAssetSet(3);
	CPUTAssetSet *hairMediumSet = mHeadAssetScene->GetAssetSet(4);
	CPUTAssetSet *hairLongSet = mHeadAssetScene->GetAssetSet(5);
	CPUTAssetSet *hairHelmet1Set = mHeadAssetScene->GetAssetSet(6);
    CPUTAssetSet *hairHelmet2Set = mHeadAssetScene->GetAssetSet(7);
    CPUTAssetSet *hairHelmet3Set = mHeadAssetScene->GetAssetSet(8);
    CPUTAssetSet *hairHelmet4Set = mHeadAssetScene->GetAssetSet(9);
	CPUTAssetSet *beardSet = mHeadAssetScene->GetAssetSet(10);

	pAssetLibrary->FindAssetSet("LandmarkMesh");

	// Load landmark positions
	int landmarkCount = landmarkSet->GetAssetCount();
	mHeadInfo.BaseHeadLandmarks.clear();
	for (int i = 1; i < landmarkCount; i++) // skip root node
	{
		CPUTRenderNode *node = NULL;
		landmarkSet->GetAssetByIndex(i, &node);
		mHeadInfo.BaseHeadLandmarks.push_back(node->GetPosition());
		int lmIdx = i - 1;
		if (lmIdx >= kLandmarkIndex_FaceOutlineStart && lmIdx <= kLandmarkIndex_FaceOutlineEnd)
		{
			// Shift the face outline landmarks so they are don't get used by the algorithm
			mHeadInfo.BaseHeadLandmarks[lmIdx].x += 100.0f; 
		}
		SAFE_RELEASE(node);
	}

	// Load landmark mesh
	landmarkMeshSet->GetAssetByIndex(1, (CPUTRenderNode**)&mCPUTLandmarkModel);
	mHeadInfo.LandmarkMesh.CopyFromDX11Mesh((CPUTMeshDX11*)mCPUTLandmarkModel->GetMesh(0));
	mHeadInfo.LandmarkMesh.ApplyTransform(mCPUTLandmarkModel->GetWorldMatrix());

	mDisplayHead = pAssetLibrary->FindModel("templateHeadModel", true);
	
	mCameraMode = CameraMode_ModelViewer;
	mCameraControlViewer = new CPUTCameraModelViewer();
	mCameraControlFPS = CPUTCameraControllerFPS::Create();
	mCameraControlOrthographic = new CPUTCameraControllerOrthographic();
	mCameraControllers[CameraMode_Free] = mCameraControlFPS;
	mCameraControllers[CameraMode_ModelViewer] = mCameraControlViewer;
	mCameraControllers[CameraMode_Orthographic] = mCameraControlOrthographic;
	
	ResetCameraDefaults();

	LoadCPUTModelToSWMesh(headSet, "Base_Head.mdl", &mBaseMesh);

	std::string headDir;
	CPUTFileSystem::GetMediaDirectory(&headDir);
	CPUTFileSystem::CombinePath(headDir, "HeadTextures", &headDir);	

	mHeadInfo.BaseHeadMesh = &mBaseMesh;
	mHeadInfo.Textures[eBaseHeadTexture_ControlMap_Displacement] = LoadTexture(headDir, "DisplacementControlMap.png");
	mHeadInfo.Textures[eBaseHeadTexture_ControlMap_Color] = LoadTexture(headDir, "ColorControlMap.png");
	mHeadInfo.Textures[eBaseHeadTexture_FeatureMap] = LoadTexture(headDir, "FeatureMap.png");
	mHeadInfo.Textures[eBaseHeadTexture_ColorTransfer] = LoadTexture(headDir, "ColorTransferMap.png");
	mHeadInfo.Textures[eBaseHeadTexture_Skin] = LoadTexture(headDir, "SkinMask.png");

	
	SMorphTweakParamDef def;

	auto QuickSet = [=](const char *category, const char *name, float defaultValue, const char *modelName, float range1, float range2, float apply1, float apply2, std::vector<SMorphTweakParamDef> &morphParamDefs)
	{
		SMorphTweakParamDef def;
		def.Reset(category, name, defaultValue);
		def.MorphParts.push_back(SMorphTweakParamPart(modelName, range1, range2, apply1, apply2));
		morphParamDefs.push_back(def);
	};

	QuickSet("Front Profile", "Head Width", 0.5f, "shape_width", 0.0f, 1.0f, -2.0f, 2.0f, mMorphParamDefs);
	QuickSet("Front Profile", "Eye Area Width", 0.5f, "shape_orbit_width", 0.0f, 1.0f, -2.0f, 2.0f, mMorphParamDefs);
	QuickSet("Front Profile", "Cheekbone Width", 0.5f, "shape_cheekbone_size", 0.0f, 1.0f, -2.0f, 2.0f, mMorphParamDefs);
	QuickSet("Front Profile", "OCC Width", 0.5f, "shape_OCC_width", 0.0f, 1.0f, -2.0f, 2.0f, mMorphParamDefs);
	QuickSet("Front Profile", "Jaw Width", 0.5f, "shape_jaw_width", 0.0f, 1.0f, -2.0f, 2.0f, mMorphParamDefs);
	QuickSet("Front Profile", "Jaw Level", 0.5f, "shape_jaw_level", 0.0f, 1.0f, -2.0f, 2.0f, mMorphParamDefs);
	QuickSet("Front Profile", "Chin Width", 0.5f, "shape_chin_width", 0.0f, 1.0f, -2.0f, 2.0f, mMorphParamDefs);
	QuickSet("Front Profile", "Neck Width", 0.5f, "shape_neck_girth", 0.0f, 1.0f, -2.0f, 2.0f, mMorphParamDefs);
		
	QuickSet("Base Shape", "Shape 1", 0.0f, "shape_1", 0.0f, 1.0f, 0.0f, 1.0f, mMorphParamDefs);
	QuickSet("Base Shape", "Shape 2", 0.0f, "shape_2", 0.0f, 1.0f, 0.0f, 1.0f, mMorphParamDefs);
	QuickSet("Base Shape", "Shape 3", 0.0f, "shape_3", 0.0f, 1.0f, 0.0f, 1.0f, mMorphParamDefs);
	QuickSet("Base Shape", "Width", 0.0f, "shape_width", 0.0f, 1.0f, 0.0f, 1.0f, mMorphParamDefs);

	def.Reset("Base Shape", "Roundness", 0.5f);
	def.MorphParts.push_back(SMorphTweakParamPart("shape_round", 0.5f, 1.0f, 0.0f, 1.0f));
	def.MorphParts.push_back(SMorphTweakParamPart("shape_square", 0.0f, 0.5f, 1.0f, 0.0f));
	mMorphParamDefs.push_back(def);
	
	def.Reset("Base Shape", "BMI", 0.5f);
	def.MorphParts.push_back(SMorphTweakParamPart("shape_BMI_Heavy", 0.5f, 1.0f, 0.0f, 1.0f));
	def.MorphParts.push_back(SMorphTweakParamPart("shape_BMI_Lean", 0.0f, 0.5f, 1.0f, 0.0f));
	mMorphParamDefs.push_back(def);

	def.Reset("Jaw", "Cheekbone", 0.0f);
	def.MorphParts.push_back(SMorphTweakParamPart("shape_Cheekbone_Size", 0.0f, 1.0f, 0.0f, 1.0f));
	mMorphParamDefs.push_back(def);

	def.Reset("Jaw", "Chin Protrude", 0.5f);
	def.MorphParts.push_back(SMorphTweakParamPart("shape_chin_back", 0.0f, 0.5f, 1.0f, 0.0f));
	def.MorphParts.push_back(SMorphTweakParamPart("shape_chin_front", 0.5f, 1.0f, 0.0f, 1.0f));
	mMorphParamDefs.push_back(def);
	
	def.Reset("Jaw", "Chin Level", 0.0f);
	def.MorphParts.push_back(SMorphTweakParamPart("shape_chin_level", 0.0f, 1.0f, 0.0f, 1.0f));
	mMorphParamDefs.push_back(def);
	
	def.Reset("Jaw", "Chin Width", 0.5f);
	def.MorphParts.push_back(SMorphTweakParamPart("shape_chin_narrow", 0.0f, 0.5f, 1.0f, 0.0f));
	def.MorphParts.push_back(SMorphTweakParamPart("shape_chin_width", 0.5f, 1.0f, 0.0f, 1.0f));
	mMorphParamDefs.push_back(def);

	def.Reset("Other", "Neck Slope", 0.0f);
	def.MorphParts.push_back(SMorphTweakParamPart("shape_chin_neck_slope", 0.0f, 1.0f, 0.0f, 1.0f));
	mMorphParamDefs.push_back(def);
	
	// Add all the morph targets available.
	for (int i = 0; i < (int)headSet->GetAssetCount(); i++)
	{
		CPUTRenderNode *node = NULL;
		headSet->GetAssetByIndex(i, &node);
		if (strstr(node->GetName().c_str(), ".mdl") && !strstr(node->GetName().c_str(), "Cshape"))
		{
			CPUTModel *model = (CPUTModel*)node;
			CPUTMeshDX11 *mesh = (CPUTMeshDX11*)model->GetMesh(0);
			if (mesh->GetVertexCount() == mBaseMesh.GetVertCount())
			{
				std::string modelName = CPUTFileSystem::basename(model->GetName(), true);
				std::string name = modelName;
				if (strncmp(name.c_str(), "shape_", 6) == 0)
				{
					name = name.substr(6);
				}

				QuickSet("All Shapes", name.c_str(), 0.5f, modelName.c_str(), 0.0f, 1.0f, -5.0f, 5.0f, mMorphParamDefs);
				QuickSet("All Shapes Post", name.c_str(), 0.5f, modelName.c_str(), 0.0f, 1.0f, -5.0f, 5.0f, mPostMorphParamDefs);
			}
		}
		SAFE_RELEASE(node);
	}

	def.Reset("Shape", "BMI", 0.5f);
	def.MorphParts.push_back(SMorphTweakParamPart("shape_BMI_Heavy", 0.5f, 1.0f, 0.0f, 1.0f));
	def.MorphParts.push_back(SMorphTweakParamPart("shape_BMI_Lean", 0.0f, 0.5f, 1.0f, 0.0f));
	mPostMorphParamDefs.push_back(def);
	QuickSet("Shape", "Ogre", 0.0f, "shape_Ogre", 0.0f, 1.0f, 0.0f, 1.0f, mPostMorphParamDefs);

	CPUTSoftwareMesh tempMesh;
	for (int i = 0; i < 2; i++)
	{
		std::vector<SMorphTweakParamDef> &list = (i == 0) ? mMorphParamDefs : mPostMorphParamDefs;
		std::vector<float> &weights = (i == 0) ? mActiveMorphParamWeights : mActivePostMorphParamWeights;
		for (auto it = list.begin(); it != list.end(); it++)
		{
			for (auto itPart = it->MorphParts.begin(); itPart != it->MorphParts.end(); itPart++)
			{
				if (mMorphTargetMap.find(itPart->MorphTargetName) == mMorphTargetMap.end())
				{
					std::string morphModelName = itPart->MorphTargetName + ".mdl";
					LoadCPUTModelToSWMesh(headSet, morphModelName.c_str(), &tempMesh);

					CMorphTarget *morphTarget = new CMorphTarget();
					BuildMorphTarget(&mBaseMesh, &tempMesh, morphTarget);
					mMorphTargetMap[itPart->MorphTargetName] = morphTarget;
				}
			}
			weights.push_back(it->Default);
		}
	}

	mHairDefs.clear();

	auto AddHair = [=](CPUTAssetSet *set, const char *modelName, const char *displayName )
	{
		SHairDef hairDef(NULL, displayName, new CPUTSoftwareMesh());
		set->GetAssetByName(modelName, (CPUTRenderNode**)&hairDef.Model);
		CPUTMeshDX11 *dx11Mesh = (CPUTMeshDX11 *)hairDef.Model->GetMesh(0);
		hairDef.SWMesh->CopyFromDX11Mesh(dx11Mesh);
		hairDef.SWMesh->ApplyTransform(hairDef.Model->GetWorldMatrix());
		mHairDefs.push_back(hairDef);
	};


	mHairDefs.push_back(SHairDef(NULL, "Hairless", NULL));

	AddHair(hairShortSet, "ShortHair.mdl", "Short");
	AddHair(hairMediumSet, "HairMedium.mdl", "Medium");
	AddHair(hairLongSet, "Long_Hair.mdl", "Long");
	AddHair(hairHelmet1Set, "hair1_retopo.mdl", "Helmet Short");
    AddHair(hairHelmet2Set, "hair2.mdl", "Helmet 2");
    AddHair(hairHelmet3Set, "hair3.mdl", "Helmet 3");
    AddHair(hairHelmet4Set, "hair4.mdl", "Helmet 4");

	mHairDefNames = (const char **)malloc(sizeof(const char*) * mHairDefs.size());
	for (int i = 0; i < (int)mHairDefs.size(); i++)
		mHairDefNames[i] = mHairDefs[i].Name.c_str();

	SetLoadHairDef(0, true);

	// Load Beard Parts
	auto AddBeardPart = [=](CPUTAssetSet *set, const char *modelName, const char *displayName)
	{
		SHairDef hairDef(NULL, displayName, new CPUTSoftwareMesh());
		beardSet->GetAssetByName(modelName, (CPUTRenderNode**)&hairDef.Model);
		CPUTMeshDX11 *dx11Mesh = (CPUTMeshDX11 *)hairDef.Model->GetMesh(0);
		hairDef.SWMesh->CopyFromDX11Mesh(dx11Mesh);
		hairDef.SWMesh->ApplyTransform(hairDef.Model->GetWorldMatrix());
		mBeardDefs.push_back(hairDef);
		mBeardEnabled.push_back(false);
	};
	
	AddBeardPart(beardSet, "Chops.mdl", "Chops");
	AddBeardPart(beardSet, "moustache.mdl", "Mustache");
	AddBeardPart(beardSet, "SideBurns.mdl", "Sideburns");
	AddBeardPart(beardSet, "SoulPatch.mdl", "Soulpatch");
	AddBeardPart(beardSet, "goatee.mdl", "Goatee");
	
	

}

void Menu_FaceMapping::SetLoadHairDef(int hairIndex, bool force)
{
	if (mCurrentHairIndex != hairIndex || force)
	{
		mCurrentHairIndex = hairIndex;
		assert(hairIndex >= 0 && hairIndex < (int)mHairDefs.size());
		SHairDef *def = &mHairDefs[hairIndex];
		if (def->Model != NULL)
		{
			mCurrentHair.CopyFrom(def->SWMesh);
		}
		mForceRebuildAll = true;
	}
}

void Menu_FaceMapping::Shutdown()
{
	SAFE_DELETE(mCameraControlViewer);
	SAFE_DELETE(mCameraControlFPS);
	SAFE_DELETE(mCameraControlOrthographic);
	SAFE_DELETE(mHeadAssetScene);
	SAFE_FREE(mHairDefNames);
	SAFE_RELEASE(mCPUTLandmarkModel);
    SAFE_RELEASE(mpCubemap);
	SAFE_DELETE(mDebugTextureSprite);
	for (int i = 0; i < (int)mHairDefs.size(); i++)
	{
		SAFE_RELEASE(mHairDefs[i].Model);
		SAFE_DELETE(mHairDefs[i].SWMesh);
	}
	for (int i = 0; i < (int)mBeardDefs.size(); i++)
	{
		SAFE_RELEASE(mBeardDefs[i].Model);
		SAFE_DELETE(mBeardDefs[i].SWMesh);
	}
	for (auto it = mMorphTargetMap.begin(); it != mMorphTargetMap.end(); it++)
	{
		SAFE_DELETE(it->second);
	}

	for (int i = 0; i < eBaseHeadTexture_Count; i++)
	{
		SAFE_RELEASE(mHeadInfo.Textures[i]);
	}
	mMorphTargetMap.clear();
}

void Menu_FaceMapping::LoadFace(const std::string &filename)
{
	mObjFilename = filename;
	mFaceModel.LoadObjFilename(filename);

	CPUTAssetLibrary *pAssetLibrary = CPUTAssetLibrary::GetAssetLibrary();
	pAssetLibrary->SetRootRelativeMediaDirectory("MyAssets");
	std::string matName = pAssetLibrary->GetMaterialDirectoryName();
	CPUTFileSystem::CombinePath(matName, "displace_map_render.mtl", &matName);

	mForceRebuildAll = true;
}

void Menu_FaceMapping::ActivationChanged(bool active)
{
	MenuBase::ActivationChanged(active);
	if (active && mIsEditingLandmarks)
	{
		gMenu_LandmarkEdit->GetOutput(&mFaceModel.Landmarks);
		mForceRebuildAll = true;
		mIsEditingLandmarks = false;
	}
}

void Menu_FaceMapping::SetDefaultTweaksInternal()
{
	SetDefaultTweaks(&mTweaks);
}

void Menu_FaceMapping::LoadFaceUI()
{
	OPENFILENAMEA ofn;
	char filename[512];
	memset(&ofn, 0, sizeof(OPENFILENAMEA));
	filename[0] = 0;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = MySample::Instance->GetHWnd();
	ofn.lpstrDefExt = ".obj";
	ofn.lpstrFilter = "Obj Files(*.obj)\0*.obj\0\0";
	std::string cwd = GetUserDataDirectory();
	ofn.lpstrInitialDir = cwd.c_str();
	ofn.lpstrTitle = "Select An DDS File";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename) / sizeof(filename[0]);
	if (GetOpenFileNameA(&ofn))
	{
		LoadFace(std::string(ofn.lpstrFile));
	}
}

void Menu_FaceMapping::HandleCPUTEvent(int eventID, int controlID, CPUTControl *control)
{
}

CPUTEventHandledCode Menu_FaceMapping::HandleKeyboardEvent(CPUTKey key, CPUTKeyState state)
{
	if (mCameraControllers[mCameraMode] != NULL)
	{
		mCameraControllers[mCameraMode]->HandleKeyboardEvent(key, state);
	}

	if (key == KEY_F1 && state == CPUT_KEY_UP)
	{
		mCameraMode = (CameraMode)((mCameraMode + 1) % CameraMode_Count);
	}
	return CPUT_EVENT_HANDLED;
}

CPUTEventHandledCode Menu_FaceMapping::HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message)
{
	if (mCameraControllers[mCameraMode] != NULL)
	{
		return mCameraControllers[mCameraMode]->HandleMouseEvent(x, y, wheel, state, message);
	}
	return CPUT_EVENT_UNHANDLED;
}

void Menu_FaceMapping::Update(float dt)
{
	MySample::Instance->mAmbientColor = float4(mAmbientLightIntensity, mAmbientLightIntensity, mAmbientLightIntensity, 1.0f);
	MySample::Instance->mLightColor = float4(mDirectionalLightIntensity, mDirectionalLightIntensity, mDirectionalLightIntensity, 1.0f);

	if (mCameraControllers[mCameraMode] != NULL)
	{
		mCameraControllers[mCameraMode]->Update(dt);
	}
}

inline float RemapRange(float value, float r1Min, float r1Max, float r2Min, float r2Max)
{
	float ratio = (value - r1Min) / (r1Max - r1Min);
	ratio = floatClamp(ratio, 0.0f, 1.0f);
	return r2Min + ratio * (r2Max - r2Min);
}

static bool PickColor(CPUTColor4 inputColor, CPUTColor4 *outputColor)
{
	*outputColor = inputColor;
	CHOOSECOLOR cc = {};
	COLORREF customColors[16];
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = MySample::Instance->GetHWnd();
	cc.rgbResult = RGB((byte)(inputColor.r * 255.0f), (byte)(inputColor.g * 255.0f), (byte)(inputColor.b * 255.0f));
	customColors[0] = RGB(228, 194, 171);
	customColors[1] = RGB(205, 50, 50);
	cc.lpCustColors = customColors;
	cc.Flags = CC_RGBINIT | CC_FULLOPEN;
	if (ChooseColor(&cc))
	{
		outputColor->r = (float)(cc.rgbResult & 0x000000ff) / 255.0f;
		outputColor->g = (float)((cc.rgbResult & 0x0000ff00) >> 8) / 255.0f;
		outputColor->b = (float)((cc.rgbResult & 0x00ff0000) >> 16) / 255.0f;
		return true;
	}
	return false;
}

static bool PromptForExportFilename(std::string &outFilename)
{
	OPENFILENAME ofn = { 0 };
	wchar_t filename[MAX_PATH] = L"sculpted_model";

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrTitle = L"Save Sculpted Obj";
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = L"Obj Files (*.obj)\0*.obj\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrDefExt = L"obj";

	char mb_obj_string[sizeof(filename) * sizeof(wchar_t)];
	WideCharToMultiByte(CP_UTF8, 0, filename, -1, mb_obj_string, sizeof(mb_obj_string) - 1, NULL, NULL);

	if (TRUE == GetSaveFileName(&ofn))
	{
		WideCharToMultiByte(CP_UTF8, 0, filename, -1, mb_obj_string, sizeof(mb_obj_string) - 1, NULL, NULL);
		outFilename = mb_obj_string;
		return true;
	}
	return false;
}

// Export the OBJ file
void Menu_FaceMapping::HandleExport()
{
	std::string outFilename;
	if (PromptForExportFilename(outFilename))
	{
		OBJExporter meshExport(outFilename);

		CPUTRenderParameters params = {};
		meshExport.ExportModel(mDisplayHead, params, 0);
		SHairDef *hairDef = (mCurrentHairIndex >= 0 && mCurrentHairIndex < (int)mHairDefs.size()) ? &mHairDefs[mCurrentHairIndex] : NULL;
		if (hairDef != NULL)
		{
			meshExport.ExportModel(hairDef->Model, params, 0);
		}
		for (int i = 0; i < (int)mBeardEnabled.size(); i++)
		{
			if (mBeardEnabled[i])
			{
				meshExport.ExportModel(mBeardDefs[0].Model, params, 0);
				break;
			}
		}

		meshExport.Close();
	}
}

void Menu_FaceMapping::UpdateLayout(CPUTRenderParameters &renderParams)
{
	mImGUIMenuWidth = fmax(250.0f, renderParams.mWidth / 3.0f);
	mViewportDim = float2((float)renderParams.mWidth - mImGUIMenuWidth, (float)renderParams.mHeight);

	mCameraControlOrthographic->SetViewportSize(mViewportDim.x, mViewportDim.y);
}

void Menu_FaceMapping::ResetActiveMorphTargets(bool post)
{
	std::vector<SMorphTweakParamDef> &list = post ? mPostMorphParamDefs : mMorphParamDefs;
	std::vector<float> &weights = post ? mActivePostMorphParamWeights : mActiveMorphParamWeights;
	for (int i = 0; i < (int)list.size(); i++)
	{
		weights[i] = list[i].Default;
	}
}

void Menu_FaceMapping::DrawGUI(CPUTRenderParameters &renderParams)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_ShowBorders;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	bool opened = true;
	float windowPadding = 0.0f;

	ImGui::SetNextWindowSize(ImVec2(mImGUIMenuWidth, renderParams.mHeight - windowPadding * 2.0f));
	ImGui::SetNextWindowPos(ImVec2(renderParams.mWidth - mImGUIMenuWidth - windowPadding, windowPadding));
	ImGui::Begin("Head Shape", &opened, window_flags);

	ImGui::Spacing();
	
	float guiSpacing = 5.0f;
	if (ImGui::CollapsingHeader("General", NULL, true, true))
	{
		if (ImGui::Button("Export", ImVec2(0, 0))) 
			HandleExport();
		ImGui::SameLine(0, guiSpacing);
		if (ImGui::Button("Reset Defaults", ImVec2(0, 0)))
		{
			SetDefaultTweaksInternal();
		}
	}
	if (ImGui::CollapsingHeader("Debug", NULL, true, false))
	{
		ImGui::Combo("Texture View", (int*)&mDebugTextureView, sDebugTextureViewNames, DebugTextureView_Count);
		if (mDebugTextureView != DebugTextureView_None)
		{
			ImGui::Indent();
			ImGui::Checkbox("Display Fullscreen", &mFullscreenDebugTextureViewer);
			if (mDebugTextureView == DebugTextureView_DisplacementColor || mDebugTextureView == DebugTextureView_DisplacementDepth)
				ImGui::Checkbox("Show Map Landmarks", &mShowMapLandmarks);
			ImGui::Unindent();
			
		}
		ImGui::Combo("Head Display Texture", (int*)&mDebugHeadDisplayTextureView, sDebugHeadDisplayTextureNames, DebugHeadDisplayTexture_Count);
		ImGui::Checkbox("Hide Skybox", &mHideCubeMap);
		ImGui::Checkbox("Wireframe", &mShowWireframe);
		ImGui::Checkbox("Show Landmark Mesh", &mRenderLandmarkMesh);
		ImGui::Checkbox("Show Morphed Landmark Mesh", &mRenderMorphedLandmarkMesh);
		ImGui::Checkbox("Show Head Landmarks", &mRenderHeadLandmarks);
		ImGui::Checkbox("Skip Fit Face", &mSkipFaceFit);
		ImGui::Checkbox("Skip Face Displace", &mSkipFaceDisplace);
		ImGui::Checkbox("Skip Face Color", &mSkipFaceColorBlend);
		ImGui::Checkbox("Skip Seam Fill", &mSkipSeamFill);

		if (ImGui::TreeNode("Lighting Options"))
		{
			ImGui::SliderFloat("Light Height", &mDirectionalLightHeight, -10.0f, 10.0f);
			ImGui::SliderAngle("Light Angle", &mDirectionalLightAngle, 0.0f);
			ImGui::SliderFloat("Direction Light", &mDirectionalLightIntensity, 0.0f, 1.0f);
			ImGui::SliderFloat("Ambient Light", &mAmbientLightIntensity, 0.0f, 1.0f);
			ImGui::TreePop();
		}

		ImGui::Combo("Camera Style", (int*)&mCameraMode, sCameraModes, CameraMode_Count);
		if (ImGui::Button("Reset Camera"))
		{
			ResetCameraDefaults();
		}
	}
	if (ImGui::CollapsingHeader("Hair", NULL, true, true))
	{
		int index = mCurrentHairIndex;
		ImGui::Combo("Hair Style", &index, mHairDefNames, (int)mHairDefs.size());
		SetLoadHairDef(index);
		if (ImGui::TreeNode("Facial Hair"))
		{
			for (int i = 0; i < (int)mBeardDefs.size(); i++)
			{
				bool enabled = mBeardEnabled[i];
				ImGui::Checkbox(mBeardDefs[i].Name.c_str(), &enabled);
				mForceRebuildAll |= mBeardEnabled[i] != enabled;
				mBeardEnabled[i] = enabled;
			}
			ImGui::TreePop();
		}

	}
	if (ImGui::CollapsingHeader("Face Scan", NULL, true, true))
	{
		if (ImGui::Button("Load Scan", ImVec2(0, 0)))
			LoadFaceUI();

		ImGui::SameLine(0, guiSpacing);

		if (IsFaceLoaded() && ImGui::Button("View Scan"))
		{
			gMenu_FaceScanPreview->LoadFaceObj(mObjFilename, true);
			gMenu_FaceScanPreview->SetFaceScanMode(FaceScanPReviewMode_ViewScan);
			MenuController_PushMenu(gMenu_FaceScanPreview);
		}
		ImGui::SameLine(0, guiSpacing);
		if (ImGui::Button("New Scan", ImVec2(0, 0)))
			MenuController_PopTo(gMenu_Scan, true);

		ImGui::SliderAngle("Yaw", &mTweaks.FaceYaw, -20.0f, 20.0f);
		ImGui::SliderAngle("Pitch", &mTweaks.FacePitch, -20.0f, 20.0f);
		ImGui::SliderAngle("Roll", &mTweaks.FaceRoll, -20.0f, 20.0f);
		ImGui::SliderFloat("Z Displace", &mTweaks.DisplaceOffset.z, -10.0f, 0.0f);
		if (IsFaceLoaded() && ImGui::Button("Edit Landmarks"))
		{
			std::vector<bool> enabledLandmarks;
			enabledLandmarks.resize(mHeadInfo.BaseHeadLandmarks.size());
			memset(&enabledLandmarks[0], 0, sizeof(bool) *mHeadInfo.BaseHeadLandmarks.size());
			for (auto itr = mPipeline.HeadGeometryStage->LandmarkMeshVertexToLandmarkIndex.begin(); itr != mPipeline.HeadGeometryStage->LandmarkMeshVertexToLandmarkIndex.end(); itr++)
				if (*itr != -1)
				{
					assert(*itr < (int)enabledLandmarks.size());
					enabledLandmarks[*itr] = true;
				}
			gMenu_LandmarkEdit->SetInput(&mFaceModel.Landmarks, &mHeadInfo.BaseHeadLandmarks, NULL, &mFaceModel, &enabledLandmarks);
			MenuController_PushMenu(gMenu_LandmarkEdit);
			mIsEditingLandmarks = true;
		}
	}
	if (ImGui::CollapsingHeader("Head Shaping", NULL, true, true))
	{
		if (ImGui::Button("Reset Shaping"))
			ResetActiveMorphTargets(false);
		bool inNode = false;
		bool nodeOpened = false;
		std::string *curCategory = NULL;
		for (int i = 0; i < (int)mMorphParamDefs.size(); i++)
		{
			SMorphTweakParamDef *def = &mMorphParamDefs[i];
			if (curCategory == NULL || *curCategory != def->Category)
			{
				if (nodeOpened)
				{
					ImGui::TreePop();
				}
				nodeOpened = ImGui::TreeNode(def->Category.c_str());
				inNode = true;
				curCategory = &def->Category;
			}
			if (nodeOpened)
				ImGui::SliderFloat(mMorphParamDefs[i].Name.c_str(), &mActiveMorphParamWeights[i], 0.0f, 1.0f);
		}
		if (nodeOpened)
			ImGui::TreePop();
	}

	if (ImGui::CollapsingHeader("Blending", NULL, true, true))
	{
		bool colorButtonClicked = false;
		ImGui::ColorEdit3("Color1", &mTweaks.BlendColor1.r, &colorButtonClicked);
		if (colorButtonClicked)
		{
			PickColor(mTweaks.BlendColor1, &mTweaks.BlendColor1);
		}
		ImGui::ColorEdit3("Color2", &mTweaks.BlendColor2.r, &colorButtonClicked);
		if (colorButtonClicked)
		{
			PickColor(mTweaks.BlendColor2, &mTweaks.BlendColor2);
		}
	}
	
	if (ImGui::CollapsingHeader("Post Head Shaping", NULL, true, true))
	{
		if (ImGui::Button("Reset Post Shaping"))
			ResetActiveMorphTargets(true);
		bool inNode = false;
		bool nodeOpened = false;
		std::string *curCategory = NULL;
		for (int i = 0; i < (int)mPostMorphParamDefs.size(); i++)
		{
			SMorphTweakParamDef *def = &mPostMorphParamDefs[i];
			if (curCategory == NULL || *curCategory != def->Category)
			{
				if (nodeOpened)
				{
					ImGui::TreePop();
				}
				nodeOpened = ImGui::TreeNode(def->Category.c_str());
				inNode = true;
				curCategory = &def->Category;
			}
			if (nodeOpened)
				ImGui::SliderFloat(mPostMorphParamDefs[i].Name.c_str(), &mActivePostMorphParamWeights[i], 0.0f, 1.0f);
		}
		if (nodeOpened)
			ImGui::TreePop();
	}
	if (ImGui::CollapsingHeader("Post Blending"), NULL, true, true)
	{
		ImGui::Combo("Post Blend Mode", (int*)&mTweaks.PostBlendMode, sPostBlendColorModes, PostBlendColorMode_Count);
		if (mTweaks.PostBlendMode == PostBlendColorMode_Adjust)
		{
			ImGui::SliderInt("Hue 1", &mTweaks.PostBlendAdjust[0].x, -180, 180);
			ImGui::SliderInt("Saturation 1", &mTweaks.PostBlendAdjust[0].y, -100, 100);
			ImGui::SliderInt("Lightness 1", &mTweaks.PostBlendAdjust[0].z, -100, 100);

			ImGui::SliderInt("Hue 2", &mTweaks.PostBlendAdjust[1].x, -180, 180);
			ImGui::SliderInt("Saturation 2", &mTweaks.PostBlendAdjust[1].y, -100, 100);
			ImGui::SliderInt("Lightness 2", &mTweaks.PostBlendAdjust[1].z, -100, 100);
		}
		else if (mTweaks.PostBlendMode == PostBlendColorMode_Colorize)
		{
			ImGui::SliderInt("Hue 1", &mTweaks.PostBlendColorize[0].x, 0, 360);
			ImGui::SliderInt("Saturation 1", &mTweaks.PostBlendColorize[0].y, 0, 100);
			ImGui::SliderInt("Lightness 1", &mTweaks.PostBlendColorize[0].z, -100, 100);

			ImGui::SliderInt("Hue 2", &mTweaks.PostBlendColorize[1].x, 0, 360);
			ImGui::SliderInt("Saturation 2", &mTweaks.PostBlendColorize[1].y, 0, 100);
			ImGui::SliderInt("Lightness 2", &mTweaks.PostBlendColorize[1].z, -100, 100);
		}
		else
		{
			// add spacing or the combobox expansion will be covered up
			for (int i = 0; i < 10; i++)
				ImGui::Spacing();
		}
	}
	if (ImGui::CollapsingHeader("Head Blending", NULL, true, true))
	{
		ImGui::TextColored( ImVec4(1.0f, 0.0f, 1.0f,1.0f), "Create a head, store it, and then blend it with another head");
		if (mDisplayHead)
		{
			if (ImGui::Button("Store Current Head"))
			{
				mOtherHead.CopyFromDX11Mesh((CPUTMeshDX11*)mDisplayHead->GetMesh(0));
				SAFE_RELEASE(mOtherHeadTexture);
				mOtherHeadTexture = mPipeline.HeadBlendStage->Output.OutputDiffuse;
				mPipeline.HeadBlendStage->Output.OutputDiffuse = NULL;
			}
			if (mOtherHeadTexture != NULL)
				ImGui::SliderFloat("Stored Head Weight", &mTweaks.OtherHeadBlend, 0.0f, 1.0f);
			else
				mTweaks.OtherHeadBlend = 0.0f;
		}
	}

	ImGui::End();

	// Create a dummy window for drawing text in the render area
	{
		bool fullOpened = true;
		ImGuiWindowFlags window_flags_full = 0;
		window_flags_full |= ImGuiWindowFlags_NoTitleBar;
		window_flags_full |= ImGuiWindowFlags_NoResize;
		window_flags_full |= ImGuiWindowFlags_NoMove;
		window_flags_full |= ImGuiWindowFlags_NoCollapse;
		window_flags_full |= ImGuiWindowFlags_NoInputs;

		ImGuiStyle& style = ImGui::GetStyle();
		float prevAlpha = style.WindowFillAlphaDefault;
		style.WindowFillAlphaDefault = 0.0f; // make it transparent
		ImGui::SetNextWindowSize(ImVec2((float)renderParams.mWidth - mImGUIMenuWidth, (float)renderParams.mHeight));
		ImGui::Begin("Fullscreen", &fullOpened, window_flags_full);

		ImGui::SetWindowPos(ImVec2(0, 0));

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Camera: %s (F1 to change)", sCameraModes[mCameraMode]);
		if (mCameraMode == CameraMode_Free)
		{
			ImGui::Text("Hold left mouse button and move mouse to look");
			ImGui::Text("W/A/S/D - Move Forward/Left/Back/Right");
			ImGui::Text("Q/E - Move Up/Down");
			ImGui::Text("Hold Shift - Move Slowly");
		}
		ImGui::End();
		style.WindowFillAlphaDefault = prevAlpha;
	}
}

void Menu_FaceMapping::CreateMorphTargetEntries(std::vector<MorphTargetEntry> &list, std::vector<SMorphTweakParamDef> &defs, std::vector<float> &weights, bool post)
{
	for (int i = 0; i < (int)defs.size(); i++)
	{
		SMorphTweakParamDef *def = &defs[i];
		float ratio = weights[i];
		for (auto part = def->MorphParts.begin(); part != def->MorphParts.end(); part++)
		{
			float weight = RemapRange(ratio, part->ParamRange0, part->ParamRange1, part->Apply0, part->Apply1);
			if (abs(weight) > 0.00001f)
			{
				MorphTargetEntry entry;
				entry.Weight = weight;
				entry.Target = mMorphTargetMap[part->MorphTargetName];
				entry.Post = post;
				list.push_back(entry);
			}
		}
	}
}

bool Menu_FaceMapping::IsFaceLoaded()
{
	return mFaceModel.GetMesh()->GetVertCount() > 0;
}

void Menu_FaceMapping::Render(CPUTRenderParameters &renderParams)
{
	UpdateLayout(renderParams);

	if(renderParams.mpShadowCamera != NULL)
	{
		float4 camPos = float4(0.0f, mDirectionalLightHeight, -10.0f, 1.0f);
		camPos = camPos * float4x4RotationY(mDirectionalLightAngle);
		renderParams.mpShadowCamera->SetPosition(camPos);
		renderParams.mpShadowCamera->LookAt(float3(0.0, 0.0f, 0.0f));
	}

	if (IsFaceLoaded())
	{
        CPUTCamera *pLastCamera = renderParams.mpCamera;
		
		SPipelineInput input;
		input.FaceModel = &mFaceModel;
		input.RenderParams = &renderParams;
		input.BaseHeadInfo = &mHeadInfo;
		input.Tweaks = &mTweaks;
		
		mTweaks.Flags = 0;
		mTweaks.Flags = mTweaks.Flags | (mSkipFaceFit ? PIPELINE_FLAG_SkipFitFace : 0);
		mTweaks.Flags = mTweaks.Flags | (mSkipFaceDisplace ? PIPELINE_FLAG_SkipDisplacmentMap : 0);
		mTweaks.Flags = mTweaks.Flags | (mSkipFaceColorBlend ? PIPELINE_FLAG_SkipFaceColorBlend : 0);
		mTweaks.Flags = mTweaks.Flags | (mSkipSeamFill ? PIPELINE_FLAG_SkipColorSeamFill : 0);

		for (int i = 0; i < kMaxHeadBlends; i++)
		{
			mTweaks.OtherHeadMesh = mOtherHeadTexture != NULL ? &mOtherHead : NULL;
			mTweaks.OtherHeadTexture = mOtherHeadTexture;
		}

		mTweaks.MorphTargetEntries.clear();
		CreateMorphTargetEntries(mTweaks.MorphTargetEntries, mMorphParamDefs, mActiveMorphParamWeights, false);
		CreateMorphTargetEntries(mTweaks.MorphTargetEntries, mPostMorphParamDefs, mActivePostMorphParamWeights, true);

		input.HairInfo.clear();
		SHairDef *hairDef = (mCurrentHairIndex >= 0 && mCurrentHairIndex < (int)mHairDefs.size()) ? &mHairDefs[mCurrentHairIndex] : NULL;
		bool hasHair = hairDef != NULL && hairDef->Model != NULL;
		if (hasHair)
		{
			input.HairInfo.push_back(SHairPipelineInfo(&mCurrentHair));
		}
		bool hasBeard = false;
		for (int i = 0; i < (int)mBeardEnabled.size(); i++)
		{
			if (mBeardEnabled[i])
			{
				input.HairInfo.push_back(SHairPipelineInfo(mBeardDefs[i].SWMesh));
				hasBeard = true;
			}
		}

		if (mForceRebuildAll || mTweaks != mLastTweaks)
		{
			mPipeline.Execute(&input, &mPipelineOutput);
			mLastTweaks = mTweaks;
			mForceRebuildAll = false;

			// Copy the deformed mesh to our template model
			mPipelineOutput.DeformedMesh->CopyToDX11Mesh((CPUTMeshDX11*)mDisplayHead->GetMesh(0));

			// Copy the deformed hair
			if (hasHair)
			{
				hairDef->Model->SetParentMatrix(float4x4Identity());
				mPipelineOutput.DeformedHairMeshes[0]->CopyToDX11Mesh((CPUTMeshDX11*)hairDef->Model->GetMesh(0));
			}
			if (hasBeard)
			{
				CPUTSoftwareMesh beardMesh;
				int beardStartIndex = hasHair ? 1 : 0;
				beardMesh.CopyFromMultiple(&mPipelineOutput.DeformedHairMeshes[beardStartIndex], (int)mPipelineOutput.DeformedHairMeshes.size() - beardStartIndex);
				if (beardMesh.GetVertCount() != 0)
				{
					mBeardDefs[0].Model->SetParentMatrix(float4x4Identity());
					beardMesh.CopyToDX11Mesh((CPUTMeshDX11*)mBeardDefs[0].Model->GetMesh(0));
				}
			}
		}

		// Copy the blend stage diffuse texture into the material
		CPUTTexture *headTextureOverride = mPipelineOutput.DiffuseTexture;
		switch (mDebugHeadDisplayTextureView)
		{
			case DebugHeadDisplayTexture_DisplacmentControlMap: { headTextureOverride = mHeadInfo.Textures[eBaseHeadTexture_ControlMap_Displacement]; } break;
			case DebugHeadDisplayTexture_ColorControlMap: { headTextureOverride = mHeadInfo.Textures[eBaseHeadTexture_ControlMap_Color]; } break;
			case DebugHeadDisplayTexture_FeatureMap: { headTextureOverride = mHeadInfo.Textures[eBaseHeadTexture_FeatureMap]; } break;
			case DebugHeadDisplayTexture_ColorTransferMap: { headTextureOverride = mHeadInfo.Textures[eBaseHeadTexture_ColorTransfer]; } break;
			case DebugHeadDisplayTexture_SkinMap: { headTextureOverride = mHeadInfo.Textures[eBaseHeadTexture_Skin]; } break;
			default: break;
		}
		CPUTMaterial *mat = mDisplayHead->GetMaterial(0, 0);
		mat->OverridePSTexture(0, headTextureOverride);
		SAFE_RELEASE(mat);
        
		renderParams.mpCamera = (CPUTCamera*)mCameraControllers[mCameraMode]->GetCamera();

		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, mViewportDim.x, mViewportDim.y, 0.0f, 1.0f };
		PUSH_VIEWPORT(CPUT_DX11::GetContext(), &viewport, 1, &renderParams);
		((CPUTCamera*)(mCameraControllers[mCameraMode]->GetCamera()))->SetAspectRatio(mViewportDim.x / mViewportDim.y);

		// Update per frame constants
		{
			CPUTFrameConstantBuffer frameConstants;
			CPUTCamera* pCamera = (CPUTCamera*)mCameraControllers[mCameraMode]->GetCamera();
			frameConstants.AmbientColor = float4(1.0) * mAmbientLightIntensity;
			frameConstants.LightColor = MySample::Instance->mLightColor;
			frameConstants.LightDirection = float4(renderParams.mpShadowCamera->GetLookWS(), 1.0);
			frameConstants.InverseView = inverse(*pCamera->GetViewMatrix());
			frameConstants.Projection = *pCamera->GetProjectionMatrix();
			renderParams.mpPerFrameConstants->SetData(0, sizeof(CPUTFrameConstantBuffer), (UINT*)&frameConstants);
		}
 
		if (!mHideCubeMap)
        {
            renderParams.mRenderOnlyVisibleModels = false;
            mpCubemap->RenderRecursive(renderParams, 0);
            renderParams.mRenderOnlyVisibleModels = true;
        }

		// render head
		mDisplayHead->Render(renderParams, 0);
		if (mShowWireframe)
			mDisplayHead->Render(renderParams, 2);

		if (hasHair)
		{
			hairDef->Model->Render(renderParams, 0);
			if (hairDef->Model->GetMaterialCount(0) > 2)
				hairDef->Model->Render(renderParams, 2);
		}
		if (hasBeard)
		{
			mBeardDefs[0].Model->Render(renderParams, 0);
		}

		if (mRenderLandmarkMesh)
		{
			mCPUTLandmarkModel->SetParentMatrix(float4x4Identity());
			mHeadInfo.LandmarkMesh.CopyToDX11Mesh((CPUTMeshDX11*)mCPUTLandmarkModel->GetMesh(0));
			mCPUTLandmarkModel->Render(renderParams, 0);
		}
		if (mRenderMorphedLandmarkMesh)
		{
			mCPUTLandmarkModel->SetParentMatrix(float4x4Identity());
			mPipeline.HeadGeometryStage->MorphedLandmarkMesh.CopyToDX11Mesh((CPUTMeshDX11*)mCPUTLandmarkModel->GetMesh(0));
			mCPUTLandmarkModel->Render(renderParams, 0);
		}

		if (mRenderHeadLandmarks)
		{
			for (int i = 0; i < (int)mHeadInfo.BaseHeadLandmarks.size(); i++)
			{
				DrawBox(renderParams, mHeadInfo.BaseHeadLandmarks[i], float3(0.25f, 0.25f, 0.25f), CPUTColor4(1.0f, 1.0f, 0.0f, 1.0f));
			}
		}


		CPUTTexture *debugTexture = NULL;
		if (mDebugTextureView != DebugTextureView_None)
		{
			switch (mDebugTextureView)
			{
			case DebugTextureView_DisplacementColor:
			{
				SetCodeTexture(0, mPipeline.DisplacementMapStage->Output.ColorMap->GetColorResourceView());
			} break;
			case DebugTextureView_DisplacementDepth:
			{
				SetCodeTexture(0, mPipeline.DisplacementMapStage->Output.DepthMap->GetColorResourceView());
			} break;
			case  DebugTextureView_FinalHeadDiffuse:
			{
				SetCodeTexture(0, mPipelineOutput.DiffuseTexture);
			} break;
			}

			float2 center = float2(0.0f, 0.0f);
			float spriteDim = 1.0f;
			// Draw overlay of displacement maps
			if (mFullscreenDebugTextureViewer)
			{
				spriteDim = floatMin((float)renderParams.mWidth, (float)renderParams.mHeight) / 1.2f - 20.0f;
				center = float2((float)renderParams.mWidth, (float)renderParams.mHeight) / 2.0f;
			}
			else
			{
				spriteDim = floatMin((float)renderParams.mWidth, (float)renderParams.mHeight) / 2.5f - 20.0f;
				center = float2(20.0f + spriteDim / 2.0f, 20.0f + spriteDim / 2.0f);// renderParams.mWidth, renderParams.mHeight) / 2.0f;
				mDebugTextureSprite->SetCoordType(SpriteCoordType_Screen);
				mDebugTextureSprite->SetTL(center.x - spriteDim / 2.0f, center.y - spriteDim / 2.0f, spriteDim, spriteDim);
			}
			mDebugTextureSprite->SetCoordType(SpriteCoordType_Screen);
			mDebugTextureSprite->SetTL(center.x - spriteDim / 2.0f, center.y - spriteDim / 2.0f, spriteDim, spriteDim);
			mDebugTextureSprite->DrawSprite(renderParams);

			if (mShowMapLandmarks && (mDebugTextureView == DebugTextureView_DisplacementColor || mDebugTextureView == DebugTextureView_DisplacementDepth))
			{
				std::vector<float2> &landmarks = mPipeline.DisplacementMapStage->Output.MapLandmarks;
				for (int i = 0; i < (int)landmarks.size(); i++)
				{
					float2 pos = center + landmarks[i] * spriteDim / 2.0f;
					DrawQuadSC(renderParams, pos, 5.0f, CPUTColor4(1.0f, 1.0f, 0.0f, 1.0f));
				}
			}
		}
	}

	DrawGUI(renderParams);
}

