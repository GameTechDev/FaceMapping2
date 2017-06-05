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
#include "MenuGlob.h"
#include "CPUTGuiControllerDX11.h"
#include "Menu_FaceScanPreview.h"


Menu_Scan *gMenu_Scan;
Menu_FaceScanPreview *gMenu_FaceScanPreview;
Menu_FaceMapping *gMenu_FaceMapping;
Menu_LandmarkEdit *gMenu_LandmarkEdit;

static int gScreenWidth = 0;
static int gScreenHeight = 0;

void MenuGlob_Init()
{
	gMenu_Scan = new Menu_Scan();
	gMenu_FaceScanPreview = new Menu_FaceScanPreview();
	gMenu_FaceMapping = new Menu_FaceMapping();
	gMenu_LandmarkEdit = new Menu_LandmarkEdit();

	gMenu_Scan->Init();
	gMenu_FaceScanPreview->Init();
	gMenu_FaceMapping->Init();
	gMenu_LandmarkEdit->Init();
}

void MenuGlob_Shutdown()
{
	gMenu_Scan->Shutdown();
	gMenu_FaceScanPreview->Shutdown();
	gMenu_FaceMapping->Shutdown();
	gMenu_LandmarkEdit->Shutdown();

	SAFE_DELETE(gMenu_Scan);
	SAFE_DELETE(gMenu_FaceScanPreview);
	SAFE_DELETE(gMenu_FaceMapping);
	SAFE_DELETE(gMenu_LandmarkEdit);
}

CPUTGuiController *MenuGlob_GUI()
{
	return (CPUTGuiController*)CPUTGuiControllerDX11::GetController();
}

void MenuGlob_GetScreenDim(int *width, int *height)
{
	*width = gScreenWidth;
	*height = gScreenHeight;
}

void MenuGlob_SetScreenDim(int width, int height)
{
	gScreenWidth = width;
	gScreenHeight = height;
}