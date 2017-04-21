/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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