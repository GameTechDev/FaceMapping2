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
#ifndef __MENU_BASE__
#define __MENU_BASE__

#include "CPUTRenderParams.h"
#include "CPUTEventHandler.h"

class CPUTControl;

class MenuBase
{

public:

	MenuBase() {}

	virtual void Resize(int width, int height ) {}

	virtual void Init() {}
	virtual void Shutdown() {}

	virtual void ActivationChanged(bool active) {}
	
	virtual void Render(CPUTRenderParameters &renderParams) {}
	virtual void Update(float dt) {}

	virtual void HandleCPUTEvent(int eventID, int controlID, CPUTControl *control) {}
	virtual CPUTEventHandledCode HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message) { return CPUT_EVENT_UNHANDLED; }
	virtual CPUTEventHandledCode HandleKeyboardEvent(CPUTKey key, CPUTKeyState state) { return CPUT_EVENT_UNHANDLED; }

protected:

};

#endif