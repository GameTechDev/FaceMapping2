/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "Menu_LandmarkEdit.h"
#include "MenuController.h"
#include "CPUTSprite.h"
#include "CPUTAssetLibrary.h"
#include "CPUT_DX11.h"
#include "CPUTModel.h"
#include "CPUTScene.h"
#include "../CFaceModel.h"
#include "../SampleUtil.h"
#include "../imgui/imgui_impl_dx11.h"

struct
{
	CPUTKey cputKey;
	float2 direction;
} gKeyData[] = 
{
	{ KEY_UP, float2(0.0f, 1.0f) },
	{ KEY_DOWN, float2(0.0f, -1.0f) },
	{ KEY_LEFT, float2(-1.0f, 0.0f) },
	{ KEY_RIGHT, float2(1.0f, 0.0f) }
};


bool CStickyButtonControl::Update(float dt)
{
	bool ret = mDown && (mTimeDown == 0.0f || mTimeDown > 0.4f);
	mTimeDown += dt;
	return ret;
}

void CStickyButtonControl::SetKey(CPUTKey key)
{
	mKey = key;
}
void CStickyButtonControl::InputEvent(CPUTKey key, CPUTKeyState state)
{
	if (key == mKey)
	{
		bool down = state == CPUT_KEY_DOWN;
		if (down != mDown)
			mTimeDown = 0.0f;
		mDown = down;
	}
}
void CStickyButtonControl::ResetState()
{
	mDown = false;
}

Menu_LandmarkEdit::Menu_LandmarkEdit()
{
	mCameraFace = CPUTCamera::Create(CPUT_ORTHOGRAPHIC);
	mCameraHead = CPUTCamera::Create(CPUT_ORTHOGRAPHIC);
	mCurrentLandmarkIndex = 0;
	mFaceModel = NULL;
	mCPUTFaceModel = NULL;
	mCPUTHeadModel = NULL;
	mSceneAssets = NULL;

	UpdateLayout(1920, 1080);
}

Menu_LandmarkEdit::~Menu_LandmarkEdit()
{
	SAFE_RELEASE(mCameraFace);
	SAFE_RELEASE(mCameraHead);
}

void Menu_LandmarkEdit::Init()
{
	CPUTAssetLibrary *pAssetLibrary = CPUTAssetLibrary::GetAssetLibrary();
	pAssetLibrary->SetRootRelativeMediaDirectory("MyAssets");
	std::string matName = pAssetLibrary->GetMaterialDirectoryName();
	CPUTFileSystem::CombinePath(matName, "landmarkedit_face.mtl", &matName);
	mFaceMaterial = CPUTMaterial::Create(matName);

	mStickyButtons.resize(ARRAYSIZE(gKeyData));
	for (int i = 0; i < (int)mStickyButtons.size(); i++)
		mStickyButtons[i].SetKey(gKeyData[i].cputKey);


	std::string mediaFilename;
	CPUTFileSystem::GetMediaDirectory(&mediaFilename);
	pAssetLibrary->SetMediaDirectoryName(mediaFilename + "\\");
	CPUTFileSystem::CombinePath(mediaFilename, "landmarkeditassets.scene", &mediaFilename);
	mSceneAssets = CPUTScene::Create();
	mSceneAssets->LoadScene(mediaFilename);

	CPUTAssetSet *headSet = mSceneAssets->GetAssetSet(0);
	SAFE_RELEASE(mCPUTHeadModel);
	headSet->GetAssetByName("landmarkEditHeadModel.mdl", (CPUTRenderNode**)&mCPUTHeadModel);

	CPUTSoftwareMesh mesh;
	mesh.CopyFromDX11Mesh((CPUTMeshDX11*)mCPUTHeadModel->GetMesh(0));
	mesh.ApplyTransform(mCPUTHeadModel->GetWorldMatrix());
	mesh.CopyToDX11Mesh((CPUTMeshDX11*)mCPUTHeadModel->GetMesh(0));

}

void Menu_LandmarkEdit::Shutdown()
{
	SAFE_DELETE(mSceneAssets);
	SAFE_RELEASE(mCPUTHeadModel);
	SAFE_RELEASE(mCPUTFaceModel);
	SAFE_RELEASE(mFaceMaterial);
}

void Menu_LandmarkEdit::Update(float dt)
{
	float2 moveDir = float2(0.0f, 0.0f);
	for (int i = 0; i < (int)mStickyButtons.size(); i++)
		moveDir = (mStickyButtons[i].Update(dt)) ? moveDir + gKeyData[i].direction : moveDir;

	if (moveDir != float2(0.0f, 0.0f))
	{
		float nudge = mShiftDown ? 0.01f : 0.1f;
		moveDir *= nudge;
		mFaceLandmarksEdited[mUsedLandmarkIndices[mCurrentLandmarkIndex]].x += moveDir.x;
		mFaceLandmarksEdited[mUsedLandmarkIndices[mCurrentLandmarkIndex]].y += moveDir.y;
	}
}

void Menu_LandmarkEdit::DrawGUI(CPUTRenderParameters &renderParams)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_ShowBorders;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	bool opened = true;
	float windowPadding = 0.0f;

	ImGui::SetNextWindowSize(ImVec2((float)mGuiWidth, (float)renderParams.mHeight - windowPadding * 2.0f));
	ImGui::SetNextWindowPos(ImVec2((float)renderParams.mWidth - mGuiWidth - windowPadding, windowPadding));
	ImGui::Begin("Head Shape", &opened, window_flags);

	ImGui::Spacing();
	std::string *curCategory = NULL;

	float guiSpacing = 5.0f;

	if (ImGui::Button("Accept", ImVec2(0, 0)))
	{
		mAcceptChanges = true;
		MenuController_PopMenu();
	}

	ImGui::SameLine(0, guiSpacing);

	if (ImGui::Button("Cancel", ImVec2(0, 0)))
	{
		mAcceptChanges = false;
		MenuController_PopMenu();
	}
	
	ImGui::SameLine(0, guiSpacing);
	if (ImGui::Button("Reset Defaults", ImVec2(0, 0)))
	{
		mFaceLandmarksEdited = mFaceModel->Landmarks;
	}

	if (mFaceLandmarksEdited.size() > 0 && mHeadLandmarks.size() > 0)
	{

		ImGui::InputInt("Landmark Index", &mCurrentLandmarkIndex, 1);
		mCurrentLandmarkIndex = intClamp(mCurrentLandmarkIndex, 0, (int)mUsedLandmarkIndices.size() - 1);

		ImGui::InputFloat3("Landmark Position", mFaceLandmarksEdited[mUsedLandmarkIndices[mCurrentLandmarkIndex]].f);
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
		ImGui::SetNextWindowSize(ImVec2((float)renderParams.mWidth - (float)mGuiWidth, (float)renderParams.mHeight));
		ImGui::Begin("Fullscreen", &fullOpened, window_flags_full);

		ImGui::SetWindowPos(ImVec2(0, 0));

		ImGui::Text("Select the landmark using the GUI on the right");
		ImGui::Text("Up/Down/Left/Right - Move selected landmark");
		ImGui::Text("Hold Shift - Move Slowly");

		ImGui::End();
		style.WindowFillAlphaDefault = prevAlpha;
	}
}

void Menu_LandmarkEdit::UpdateLayout(float width, float height)
{
	float viewportSpacing = 10.0f;
	mGuiWidth = fmax(250.0f, width / 4.0f);

	float workingWidth = width - mGuiWidth;
	workingWidth -= viewportSpacing * 3.0f;
	float viewportWidth = workingWidth / 2.0f;
	float viewportHeight = height - viewportSpacing * 2.0f;

	mViewportFace.TopLeftX = viewportSpacing;
	mViewportFace.TopLeftY = viewportSpacing;
	mViewportFace.Width = viewportWidth;
	mViewportFace.Height = viewportHeight;
	mViewportFace.MinDepth = 0.0f;
	mViewportFace.MaxDepth = 1.0f;

	mViewportHead = mViewportFace;
	mViewportHead.TopLeftX = viewportSpacing * 2.0f + viewportWidth;

	float aspectRatio = viewportWidth / viewportHeight;
	mCameraFace->SetWidth(10.0f);
	mCameraFace->SetHeight(10.0f / aspectRatio);
	mCameraFace->SetPosition(0.0f, 0.0f, -10.0f);
	mCameraFace->LookAt(0.0f, 0.0f, 0.0f);
	mCameraFace->Update();

	mFaceWorldMatrix = float4x4Scale(1.0f, -1.0f, 1.0f);
	
	
	mHeadWorldMatrix = float4x4Scale(0.3f, -0.3f, 0.3f);
}

void Menu_LandmarkEdit::Render(CPUTRenderParameters &renderParams)
{
	UpdateLayout((float)renderParams.mWidth, (float)renderParams.mHeight);

	ID3D11DeviceContext *context = CPUT_DX11::GetContext();

	{
		PUSH_VIEWPORT(context, &mViewportFace, 1, &renderParams);
		renderParams.mpCamera = mCameraFace;
		mCPUTFaceModel->SetParentMatrix(mFaceWorldMatrix);
		mCPUTFaceModel->Render(renderParams, 0);

		for (int i = 0; i < (int)mUsedLandmarkIndices.size(); i++)
		{
			float3 pos = mFaceLandmarksEdited[mUsedLandmarkIndices[i]];
			pos = float4(pos.x, pos.y, pos.z, 1.0f) * mFaceWorldMatrix;
			CPUTColor4 color = (mCurrentLandmarkIndex == i) ? CPUTColor4(1.0f, 0.0f, 0.0f, 1.0f) : CPUTColor4(1.0f, 1.0f, 0.0f, 1.0f);
			pos.z -= 1.0f; // bring the landmarks back on the z axis a bit so they don't get occluded
			DrawCube(renderParams, pos, 0.1f, color);
		}
	}

	{
		PUSH_VIEWPORT(context, &mViewportHead, 1, &renderParams);
		mCPUTHeadModel->SetParentMatrix(mHeadWorldMatrix);
		mCPUTHeadModel->Render(renderParams, 0);

		for (int i = 0; i < (int)mUsedLandmarkIndices.size(); i++)
		{
			float3 pos = mHeadLandmarks[mUsedLandmarkIndices[i]];
			pos = float4(pos.x, pos.y, pos.z, 1.0f) *mHeadWorldMatrix;
			CPUTColor4 color = (mCurrentLandmarkIndex == i) ? CPUTColor4(1.0f, 0.0f, 0.0f, 1.0f) : CPUTColor4(1.0f, 1.0f, 0.0f, 1.0f);
			pos.z -= 1.0f;
			DrawCube(renderParams, pos, 0.1f, color);
		}
	}

	DrawGUI(renderParams);
}


void Menu_LandmarkEdit::SetInput(std::vector<float3> *faceCurrentLandmarks, std::vector<float3> *headLandmarks, CPUTModel *headModel, CFaceModel *faceModel, std::vector<bool> *landmarkUsedMask)
{
	mFaceLandmarksEdited = *faceCurrentLandmarks;
	mHeadLandmarks = *headLandmarks;
	mFaceModel = faceModel;
	if (headModel != NULL)
	{
		SAFE_RELEASE(mCPUTHeadModel);
		headModel = mCPUTHeadModel;
		headModel->AddRef();
	}
	
	SAFE_RELEASE(mCPUTFaceModel);
	mCPUTFaceModel = faceModel->CreateCPUTModel();
	mFaceMaterial->OverridePSTexture(0, faceModel->GetTexture());
	mCPUTFaceModel->SetMaterial(0, &mFaceMaterial, 1);

	mUsedLandmarkIndices.clear();
	if (landmarkUsedMask != NULL)
	{
		for (int i = 0; i < (int)landmarkUsedMask->size(); i++)
		{
			if ((*landmarkUsedMask)[i])
				mUsedLandmarkIndices.push_back(i);
		}
	}
	else
	{
		for (int i = 0; i < (int)mHeadLandmarks.size(); i++)
			mUsedLandmarkIndices.push_back(i);
	}
}

CPUTEventHandledCode Menu_LandmarkEdit::HandleKeyboardEvent(CPUTKey key, CPUTKeyState state) 
{ 
	for (auto btn = mStickyButtons.begin(); btn != mStickyButtons.end(); btn++)
		btn->InputEvent(key, state);

	if (key == KEY_SHIFT)
	{
		mShiftDown = state == CPUT_KEY_DOWN;
	}

	return CPUT_EVENT_UNHANDLED; 
}

void Menu_LandmarkEdit::ActivationChanged(bool active)
{
	for (auto btn = mStickyButtons.begin(); btn != mStickyButtons.end(); btn++)
		btn->ResetState();

	mShiftDown = false;
}

bool Menu_LandmarkEdit::GetOutput(std::vector <float3> *newLandmarks)
{
	if (mAcceptChanges)
	{
		*newLandmarks = mFaceLandmarksEdited;
	}
	return mAcceptChanges;
}