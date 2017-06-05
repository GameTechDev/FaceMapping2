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
#include "MenuButton.h"

MenuButton::MenuButton()
{
	mSize.width = 200;
	mSize.height = 100;
}

void MenuButton::Render()
{

	MenuRectF rect = GetRectangleA();
	float scale = 1.0f;

	if (mText.size() != 0 )
	{
		float2 textDim;
		Font_Measure(font, mText, &textDim, scale);
		
		float2 pos = GetCenterA() - textDim / 2.0f;

		Font_Render(font, TEXT, pos, color, scale);
	}

}