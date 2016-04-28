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