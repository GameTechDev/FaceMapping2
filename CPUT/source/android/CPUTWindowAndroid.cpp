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
#include "CPUTWindowAndroid.h"
#include "CPUT_OGL.h"

#include <unistd.h>

#include <android_native_app_glue.h>

// static initializers
bool CPUTWindowAndroid::mbMaxMinFullScreen=false;


android_app*    CPUTWindowAndroid::mpAppState = NULL;
std::vector<std::function<void(int width, int height)>> CPUTWindow::mResizeEventCallbacks;
std::vector<std::function<CPUTEventHandledCode(CPUTKey key, CPUTKeyState state)>> CPUTWindow::mKeyboardEventCallbacks;
std::vector<std::function<CPUTEventHandledCode(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message)>> CPUTWindow::mMouseEventCallbacks;
std::vector<std::function<void()>> CPUTWindow::mLoopEventCallbacks;


// Constructor
//-----------------------------------------------------------------------------
CPUTWindowAndroid::CPUTWindowAndroid()
{
    mWidth = 0;
    mHeight = 0;
}

// Destructor
//-----------------------------------------------------------------------------
CPUTWindowAndroid::~CPUTWindowAndroid()
{
    mWidth = 0;
    mHeight = 0;
}


// Create window
// 
//-----------------------------------------------------------------------------
CPUTResult CPUTWindowAndroid::Create(const std::string WindowTitle, CPUTWindowCreationParams windowParams)
{
    ASSERT(mpAppState, "mpAppState not set in startup code");

    DEBUG_PRINT("CPUTWindowAndroid::Create\n");

    return CPUT_SUCCESS;
}



// Destroy window
//-----------------------------------------------------------------------------
int CPUTWindowAndroid::Destroy()
{
    
    DEBUG_PRINT("CPUTWindowAndroid::Destroy\n");

    return true;
}

NativeWindowType CPUTWindowAndroid::GetHWnd()
{ 
	return mpAppState->window;
}




// start main message loop
//-----------------------------------------------------------------------------
int CPUTWindowAndroid::StartMessageLoop()
{
    // We might not have a window yet, so can't call it to start the message loop
    // It must be done here.

	 DEBUG_PRINT("CPUTWindowAndroid::StartMessageLoop\n");
    // We can create event queues here if we are interested in Sensors etc.
    // They would drop out of the source->process code and require special processing
    while (1)
    {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
//        while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events, (void**)&source)) >= 0) 
// TODO "Need to not poll if we don't have focus"
        while ((ident=ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) 
        {
            // Process this event.
            if (source != NULL) 
            {
                source->process(mpAppState, source);
            }
            
            if (mpAppState->destroyRequested)
            {
                DEBUG_PRINT("Destroy Requested");
                return 0;
            }
        }
        
        for (const auto &callBack : mLoopEventCallbacks) {
            callBack();
        }
    }
}