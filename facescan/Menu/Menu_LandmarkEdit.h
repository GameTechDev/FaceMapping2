/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "MenuBase.h"
#include <vector>
#include "CPUT_DX11.h"

class CStickyButtonControl
{
public:
	bool Update(float dt);
	void SetKey(CPUTKey key);
	void InputEvent(CPUTKey key, CPUTKeyState state);
	void ResetState();

private:
	CPUTKey mKey;
	float mTimeDown;
	bool mDown;
};

class CFaceModel;
class CPUTScene;

class EditViewport
{
public:
	D3D11_VIEWPORT viewport;
};

class Menu_LandmarkEdit : public MenuBase
{
public:

	Menu_LandmarkEdit();
	~Menu_LandmarkEdit();

	virtual void Init();
	virtual void Shutdown();

	virtual void Update(float dt);
	virtual void Render(CPUTRenderParameters &renderParams);
	virtual CPUTEventHandledCode HandleKeyboardEvent(CPUTKey key, CPUTKeyState state);
	virtual void ActivationChanged(bool active);

	void SetInput(std::vector<float3> *faceCurrentLandmarks, std::vector<float3> *headLandmarks, CPUTModel *headModel, CFaceModel *faceModel, std::vector<bool> *LandmarkUsedMask);

	bool GetOutput(std::vector <float3> *newLandmarks);

private:

	void UpdateLayout(float width, float height);
	void DrawGUI(CPUTRenderParameters &params);
	
	std::vector<int> mUsedLandmarkIndices;
	std::vector<float3> mFaceLandmarksEdited;
	
	std::vector<float3> mHeadLandmarks;
	CPUTModel *mCPUTHeadModel;
	CPUTModel *mCPUTFaceModel;
	float4x4 mFaceWorldMatrix;
	float4x4 mHeadWorldMatrix;

	CFaceModel *mFaceModel;

	D3D11_VIEWPORT mViewportFace;
	D3D11_VIEWPORT mViewportHead;
	float mGuiWidth;

	int mCurrentLandmarkIndex;

	CPUTMaterial *mFaceMaterial;

	CPUTCamera *mCameraFace;
	CPUTCamera *mCameraHead;

	std::vector<CStickyButtonControl> mStickyButtons;
	bool mShiftDown;

	CPUTScene *mSceneAssets;

	bool mAcceptChanges;
	
};