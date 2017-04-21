/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __MENU_FACESCANPREVIEW__
#define __MENU_FACESCANPREVIEW__

#include "MenuBase.h"
#include "CPUTSprite.h"
#include "../SampleUtil.h"
#include "CPUTCamera.h"
#include "CPUTAssetSet.h"
#include "CPUTCheckbox.h"
#include "../CFaceModel.h"
#include <string>

enum FaceScanPreviewMode
{
	FaceScanPreviewMode_ApproveScan,
	FaceScanPReviewMode_ViewScan
};

class Menu_FaceScanPreview : public MenuBase
{
public:

	Menu_FaceScanPreview();
	~Menu_FaceScanPreview();

	virtual void Init();

	void Update(float dt);
	void Render(CPUTRenderParameters &renderParams);

	void SetFaceScanMode(FaceScanPreviewMode mode);

	void LoadFaceObj(std::string filename, bool absoluteFilename = false, bool forceReload = false);

	virtual void HandleCPUTEvent(int eventID, int controlID, CPUTControl *control);
	virtual CPUTEventHandledCode HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message);
	virtual void ActivationChanged(bool active);

private:

	CPUTModel *mBoxModel;
	CPUTAssetSet *mCommonSet;

	CPUTModel *displayModel;
	CPUTCameraModelViewer *cameraController;

	std::string mModelFilename;

	CFaceModel mFaceModel;

	CPUTCheckbox *mLandmarkCheckbox;

	FaceScanPreviewMode mMode;

	void MoveUserData(const char *srcObjFilename, const char *dstObjFilename);
};


#endif