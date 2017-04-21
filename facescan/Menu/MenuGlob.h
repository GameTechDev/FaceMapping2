/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __MENU_GLOB__
#define __MENU_GLOB__

#include "Menu_Scan.h"
#include "Menu_FaceScanPreview.h"
#include "Menu_FaceMapping.h"
#include "Menu_LandmarkEdit.h"

#define MENU_CPUT_PANEL_ID 100

extern Menu_Scan *gMenu_Scan;
extern Menu_FaceScanPreview *gMenu_FaceScanPreview;
extern Menu_FaceMapping *gMenu_FaceMapping;
extern Menu_LandmarkEdit *gMenu_LandmarkEdit;

void MenuGlob_Init();
void MenuGlob_Shutdown();

class CPUTGuiController;
CPUTGuiController *MenuGlob_GUI();

void MenuGlob_GetScreenDim(int *width, int *height);
void MenuGlob_SetScreenDim(int width, int height);

#endif