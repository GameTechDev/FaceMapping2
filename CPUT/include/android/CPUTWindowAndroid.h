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
