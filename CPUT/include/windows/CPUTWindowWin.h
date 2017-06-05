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
#ifndef __WINDOWWIN_H__
#define __WINDOWWIN_H__

#include "CPUT.h"

#include "CPUTOSServices.h"
#include "CPUTResource.h" // win resource.h customized for CPUT
#include "CPUTWindow.h"
#include <winuser.h> // for character codes
#include <string>

typedef LRESULT (*WinProcHookFunc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// OS-specific window class
//-----------------------------------------------------------------------------
class CPUTWindowWin : public CPUTWindow
{
public:
    // construction
    CPUTWindowWin();
    virtual ~CPUTWindowWin();

    // Creates a graphics-context friendly window
    virtual CPUTResult Create(const std::string WindowTitle, CPUTWindowCreationParams windowParams);

    // Main windows message loop that handles and dispatches messages
    int StartMessageLoop();
    int Destroy();
    int ReturnCode();

    // return the HWND/Window handle for the created window
    HWND GetHWnd() { return mhWnd; };

    // screen/window dimensions
    void GetClientDimensions( int *pWidth, int *pHeight);
    void GetClientDimensions( int *pX, int *pY, int *pWidth, int *pHeight);
    void GetDesktopDimensions(int *pX, int *pY, int *pWidth, int *pHeight);
    bool IsWindowMaximized();
    bool IsWindowMinimized();
    bool DoesWindowHaveFocus();
    void SetFullscreenState(bool fullscreen);
    bool GetFullscreenState();
    void GetWindowDimensions(int *pX, int *pY, int *pWidth, int *pHeight);
    
    // Mouse capture - 'binds'/releases all mouse input to this window
    virtual void CaptureMouse();
    virtual void ReleaseMouse();

    virtual void SetAlwaysOnTop(bool alwaysOnTop);
    virtual bool IsTopmost();

	static void SetWinProcOverride(WinProcHookFunc callback) {
		mWinProcCallback = callback;
	}

protected:

	static WinProcHookFunc mWinProcCallback;

    HINSTANCE           mhInstance;                // current instance
    HWND                mhWnd;                     // window handle

    bool                mFullscreen;               // Is in fullscreen mode?
    RECT                mWindowedRect;
    int                 mAppClosedReturnCode;      // windows OS return code
    std::string         mAppTitle;                 // title put at top of window

    static bool         mbMaxMinFullScreen;
    DWORD               mWindowedStyle;

    // Window creation helper functions
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // CPUT conversion helper functions
    static CPUTMouseState ConvertMouseState(WPARAM wParam);
    static CPUTKey ConvertVirtualKeyToCPUTKey(WPARAM wParam);
    static CPUTKey ConvertCharacterToCPUTKey(WPARAM wParam);
};


#endif //#ifndef __WINDOWWIN_H__
