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
#ifndef __MENU_SCAN__
#define __MENU_SCAN__

#include "MenuBase.h"
#include "CPUTSprite.h"
#include "../SampleUtil.h"
#include "CPUTText.h"

#ifndef DISABLE_RSSDK

enum ScanState
{
	ScanState_ColorFeed,
	ScanState_Scanning,
	ScanState_ScanComplete,
};

class Menu_Scan : public MenuBase
{
public:

	virtual void Init();
	virtual void Shutdown();

	void Update(float dt);
	void Render(CPUTRenderParameters &renderParams);

	virtual void HandleCPUTEvent(int eventID, int controlID, CPUTControl *control);
	virtual void ActivationChanged(bool active);

private:

	void SetState(ScanState state);

	ScanState mScanState;

	SCodeTextureWrap	CameraColorTexture;
	SCodeTextureWrap	ScanPreviewTexture;

	CPUTSprite *mFullscreenSprite;

	std::vector<CPUTText *> mStatusMessages;
	std::vector<CPUTText *> mHeadMessages;

	CPUTButton *mBeginScanButton;
	
	void AddMessageControls();
};

#else
class Menu_Scan : public MenuBase
{

};
#endif


#endif