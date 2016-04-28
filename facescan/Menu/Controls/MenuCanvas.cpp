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
