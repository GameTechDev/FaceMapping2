/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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