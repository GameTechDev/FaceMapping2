/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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