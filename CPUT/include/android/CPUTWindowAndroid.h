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
#ifndef CPUTWINDOWANDROID_H
#define CPUTWINDOWANDROID_H

#include <string.h>

#include "CPUT.h"
#include "CPUTOSServices.h"
#include "CPUTWindow.h"
//#include "CPUTResource.h" // win resource.h customized for CPUT
#include <vector>
#include <string>
#include <functional>
using namespace std;
 
 
#define HWND int
#define ATOM int
#define LPARAM int
#define WPARAM int
#define HINSTANCE int
#define LRESULT int

// OS-specific window class
//-----------------------------------------------------------------------------
class CPUTWindowAndroid : public CPUTWindow
{
	static struct android_app        *mpAppState;
public:
    // construction
    CPUTWindowAndroid();
    ~CPUTWindowAndroid();

	static void SetAppState(struct android_app        *pAppState){mpAppState = pAppState;}
	static struct android_app* GetAppState(){ return mpAppState;}

    // Creates a graphics-context friendly window
    virtual CPUTResult Create(const std::string WindowTitle, CPUTWindowCreationParams windowParams);

    void GetClientDimensions(int *pX, int *pY, int *pWidth, int *pHeight) 
    {
        *pX = 0; *pY = 0; 
		*pWidth  = mWidth;
		*pHeight = mHeight;
	};
    void GetClientDimensions( int *pWidth, int *pHeight)
   {
    *pWidth  = mWidth;
    *pHeight = mHeight;
	};
   
	void SetClientDimensions( int Width, int Height)
    {
		  mWidth = Width;
		  mHeight = Height;
	};

    // Main windows message loop that handles and dispatches messages
    int StartMessageLoop();
    int Destroy();

    // return the HWND/Window handle for the created window
    NativeWindowType GetHWnd(); 

protected:

    int                mWidth;
    int                mHeight;
    static bool        mbMaxMinFullScreen;

};


#endif // CPUTWINDOWANDROID_H
