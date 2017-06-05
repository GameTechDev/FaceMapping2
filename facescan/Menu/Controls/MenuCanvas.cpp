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
#include "MenuCanvas.h"


void MenuCanvas::AddControl( MenuControl *control )
{
	control->SetParent(this);
	mControls.push_back(control);
}

void MenuCanvas::RemoveControl(MenuControl *control)
{
	for (int i = 0; i < mControls.size(); i++ )
	{
		if ( mControls[i] == control )
		{
			mControls.erase( mControls.begin() + i );
			break;
		}
	}
}

void MenuCanvas::Render()
{
	for (int i = 0; i < mControls.size(); i++)
	{
		mControls[i]->Render();
	}
}

void MenuCanvas::Process(float dt)
{
	for (int i = 0; i < mControls.size(); i++)
	{
		mControls[i]->Render();
	}
}
