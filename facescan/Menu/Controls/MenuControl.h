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
#include "CPUTMath.h"
#include "MenuTypes.h"

class MenuControl
{
public:

	MenuControl();

	void SetParent(MenuControl *parent) { mParent = parent; }
	MenuControl *GetParent() { return mParent; }

	MenuSizeF GetSize() { return mSize; }
	MenuPointF GetPos() { return mPos; }
	MenuPointF GetCenter() { return MenuPointF(mPos.x + mSize.width / 2.0f, mPos.y + mSize.height / 2.0f); }
	MenuRectF GetRectangle();

	MenuPointF GetCenterA();
	MenuPointF GetPosA();
	MenuRectF GetRectangleA();

	virtual void Render() { }
	virtual void Process(float dt) { }
	virtual void PreRender() { }

protected:

	MenuControl *mParent;

	MenuPointF mPos;
	MenuSizeF mSize;
	
};