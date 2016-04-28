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
