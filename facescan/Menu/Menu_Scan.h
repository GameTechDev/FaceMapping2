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