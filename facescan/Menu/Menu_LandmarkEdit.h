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