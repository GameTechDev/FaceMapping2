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
#ifndef CPUTWINDOWX_H
#define CPUTWINDOWX_H

#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <functional>
#include <vector>

#include "CPUTWindow.h"
#include "CPUT.h"
#include "CPUTOSServices.h"
//#include "CPUTResource.h" // win resource.h customized for CPUT

 
#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

#define HDC int
#define HGLRC int
#define HWND int
#define ATOM int
#define LPARAM int
#define WPARAM int
#define HINSTANCE int
#define LRESULT int



// OS-specific window class
//-----------------------------------------------------------------------------
class CPUTWindowX : public CPUTWindow
{
public:
    // construction
    CPUTWindowX();
    ~CPUTWindowX();

    // Creates a graphics-context friendly window
    virtual CPUTResult Create(const std::string WindowTitle, CPUTWindowCreationParams windowParams);

    virtual void GetClientDimensions(int *pWidth, int *pHeight);
    void GetClientDimensions(int *pX, int *pY, int *pWidth, int *pHeight) {
        *pX = 0; *pY = 0;  GetClientDimensions(pWidth, pHeight);
    };
    void Present();
      
    // Main windows message loop that handles and dispatches messages
    int StartMessageLoop();
    int Destroy();
    int ReturnCode();
    void RegisterCallbackResizeEvent(std::function<void(int width, int height)> callback) { mResizeEventCallbacks.push_back(callback); }
    void RegisterCallbackKeyboardEvent(std::function<CPUTEventHandledCode(CPUTKey key, CPUTKeyState state)> callback) { mKeyboardEventCallbacks.push_back(callback); }
    void RegisterCallbackMouseEvent(std::function<CPUTEventHandledCode(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message)> callback) { mMouseEventCallbacks.push_back(callback); }
    void RegisterLoopEvent(std::function<void()> callback) { mLoopEventCallbacks.push_back(callback); }

    // return the HWND/Window handle for the created window
    HWND GetHWnd() { return mhWnd;};

protected:
    Display            *pDisplay;
    Window              win;
    Atom                wmDeleteMessage;
    GLXContext          ctx;
    HINSTANCE           mhInst;                    // current instance
    HWND                mhWnd;                     // window handle
    int                 mAppClosedReturnCode;      // windows OS return code
    std::string         mAppTitle;                 // title put at top of window

    static bool         mbMaxMinFullScreen;

    // Window creation helper functions
    ATOM MyRegisterClass(HINSTANCE hInstance);
    bool InitInstance(int nCmdShow, int windowWidth, int windowHeight, int windowX, int windowY);
    static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // CPUT conversion helper functions
    static CPUTMouseState ConvertMouseState(XButtonEvent *pMouseEvent);
    static CPUTKey ConvertKeyCode(XKeyEvent *pKeyEvent);
    static CPUTKey ConvertSpecialKeyCode(WPARAM wParam, LPARAM lParam);
};


#endif // CPUTWINDOWX_H
