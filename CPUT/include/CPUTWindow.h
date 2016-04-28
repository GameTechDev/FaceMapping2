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
#ifndef __CPUTWINDOW_H__
#define __CPUTWINDOW_H__

#include <functional>
#include <vector>

// abstract base window class
//-----------------------------------------------------------------------------
class CPUTWindow
{
public:
    // construction
    CPUTWindow(){};
    virtual ~CPUTWindow(){};

    virtual CPUTResult Create(const std::string WindowTitle, CPUTWindowCreationParams windowParams) = 0;

    // screen/window dimensions
    virtual void GetClientDimensions( int *pWidth, int *pHeight) = 0;
    virtual void GetClientDimensions( int *pX, int *pY, int *pWidth, int *pHeight) = 0;
    virtual void SetClientDimensions( int Width, int Height){};
    virtual void SetFullscreenState(bool fullscreen){};
    virtual bool GetFullscreenState() { return true; };
    virtual bool DoesWindowHaveFocus() { return true; };
    virtual void SetAlwaysOnTop(bool alwaysOnTop) {};
    virtual bool IsTopmost() { return false; };

    void RegisterCallbackResizeEvent(std::function<void(int width, int height)> callback) { mResizeEventCallbacks.push_back(callback); }
    void RegisterCallbackKeyboardEvent(std::function<CPUTEventHandledCode(CPUTKey key, CPUTKeyState state)> callback) { mKeyboardEventCallbacks.push_back(callback); }
    void RegisterCallbackMouseEvent(std::function<CPUTEventHandledCode(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message)> callback) { mMouseEventCallbacks.push_back(callback); }
    void RegisterLoopEvent(std::function<void()> callback) { mLoopEventCallbacks.push_back(callback); }

    static std::vector<std::function<void(int width, int height)>> mResizeEventCallbacks;  // resize window callbacks
    static std::vector<std::function<CPUTEventHandledCode(CPUTKey key, CPUTKeyState state)>> mKeyboardEventCallbacks;  // keyboard event callbacks
    static std::vector<std::function<CPUTEventHandledCode(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message)>> mMouseEventCallbacks;  // mouse event callbacks
    static std::vector<std::function<void()>> mLoopEventCallbacks;  // wndproc looping callbacks
};

#endif