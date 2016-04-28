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
#ifndef __MENU_CONTROLLER__
#define __MENU_CONTROLLER__

#include "MenuBase.h"

class CPUTControl;

void MenuController_Init();

void MenuController_PushMenu(MenuBase *menu, bool deferToUpdate=true);

void MenuController_PopMenu(bool deferToUpdate = true);
void MenuController_PopTo(MenuBase *menu, bool deferToUpdate = true);

void MenuController_Render(CPUTRenderParameters &renderParams);
void MenuController_Update(float dt);

void MenuController_Resize(int width, int height);

void MenuController_HandleCPUTEvent(int event, int controlID, CPUTControl * pControl);
CPUTEventHandledCode MenuController_HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message);
CPUTEventHandledCode MenuController_HandleKeyboardEvent(CPUTKey key, CPUTKeyState state);

#endif