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
#ifndef __CPUT_SAMPLESTARTDX11_H__
#define __CPUT_SAMPLESTARTDX11_H__

#include <stdio.h>
#include <time.h>
#include "CPUTSprite.h"
#if defined(CPUT_FOR_OGL) || defined(CPUT_FOR_OGLES)
#include "CPUT_OGL.h"
#include "CPUTRenderTargetOGL.h"
#endif

#ifdef CPUT_FOR_DX11
#include "CPUT_DX11.h"
#include <D3D11.h>
#include "CPUTBufferDX11.h"
#endif

#include "SampleUtil.h"

#include "CPUTSprite.h"
#include "CPUTScene.h"
#include "CPUTParser.h"
#include "CPUTTextureDX11.h"

// define some controls
#define FPS_VALUES 64
const CPUTControlID ID_MAIN_PANEL = 10;
const CPUTControlID ID_SECONDARY_PANEL = 20;
const CPUTControlID ID_FULLSCREEN_BUTTON = 100;
const CPUTControlID ID_NEXTMODEL_BUTTON = 101;
const CPUTControlID ID_TEST_CONTROL = 1000;
const CPUTControlID ID_IGNORE_CONTROL_ID = -1;

//-----------------------------------------------------------------------------
#ifdef CPUT_FOR_DX11
class MySample : public CPUT_DX11
#endif
#if defined(CPUT_FOR_OGL) || defined(CPUT_FOR_OGLES)
class MySample : public CPUT_OGL
#endif
{
public:
	static MySample *Instance;

private:

	CPUTSprite			*mpFullscreenSprite;

    float                  mfElapsedTime;
    CPUTCameraController  *mpCameraController;
    CPUTSprite            *mpDebugSprite;
    CPUTAssetSet          *mpShadowCameraSet;
    CPUTText              *mpText;
    CommandParser          mParsedCommandLine;
    CPUTRenderTargetDepth *mpShadowRenderTarget;

public:
    MySample() : 
        mfElapsedTime(0.0),
        mpCameraController(NULL),
        mpDebugSprite(NULL),
        mpShadowCameraSet(NULL),
        mpText(NULL),
        mpShadowRenderTarget(NULL)
    {
		Instance = this;
		
    }
    virtual ~MySample()
    {   
    }
    void ReleaseResources();
    void CreateResources();
    void BuildGUI();

    virtual CPUTEventHandledCode HandleKeyboardEvent(CPUTKey key, CPUTKeyState state);
    virtual CPUTEventHandledCode HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message);
    virtual void                 HandleCallbackEvent( CPUTEventID Event, CPUTControlID ControlID, CPUTControl *pControl );
    
    virtual void Create();
	virtual void Shutdown();
    virtual void Render(double deltaSeconds);
    virtual void Update(double deltaSeconds);
    virtual void ResizeWindow(UINT width, UINT height);

    void LoadAssets();
    void SetCommandLineArguments(const CommandParser& mParsedCommandLine);

};
#endif // __CPUT_SAMPLESTARTDX11_H__
