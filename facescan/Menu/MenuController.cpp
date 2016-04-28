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
#include "MenuController.h"
#include "MenuGlob.h"
#include <stack>
#include "CPUTGuiController.h"

enum DeferAction
{
	DeferAction_None,
	DeferAction_Push,
	DeferAction_Pop,
	DeferAction_PopTo
};

struct DeferEvent
{
	DeferAction action;
	MenuBase *p1;
};

struct SMenuControllerGlob
{
	std::stack<MenuBase*> MenuStack;

	DeferEvent deferEvent;

};
SMenuControllerGlob gController;

void MenuController_Init()
{
	gController.deferEvent.action = DeferAction_None;
}

static void FireActivationEvents(MenuBase *activeMenu, MenuBase *nextMenu)
{
	MenuGlob_GUI()->DeleteControlsFromPanel(MENU_CPUT_PANEL_ID);

	if (activeMenu != NULL)
	{
		activeMenu->ActivationChanged(false);
	}
	if (nextMenu != NULL)
	{
		nextMenu->ActivationChanged(true);
	}
}

static MenuBase *GetActiveMenu()
{
	return gController.MenuStack.empty() ? NULL : gController.MenuStack.top();
}

static void MenuController_ExecuteDeferredEvent(DeferEvent *deferEvent)
{
	switch (deferEvent->action)
	{
	case DeferAction_Push:
	{
		MenuController_PushMenu(deferEvent->p1, false);
	} break;
	case DeferAction_Pop:
	{
		MenuController_PopMenu( false );
	} break;
	case DeferAction_PopTo:
	{
		MenuController_PopTo(deferEvent->p1, false );
	} break;
	}
}

void MenuController_PushMenu( MenuBase *menu, bool defer )
{
	if ( defer )
	{ 
		gController.deferEvent.action = DeferAction_Push;
		gController.deferEvent.p1 = menu;
	}
	else
	{
		FireActivationEvents(GetActiveMenu(), menu);
		gController.MenuStack.push(menu);
	}
}

void MenuController_PopMenu( bool defer )
{
	if (defer)
	{
		gController.deferEvent.action = DeferAction_Pop;
	}
	else
	{
		int stackCount = (int)gController.MenuStack.size();

		MenuBase *activeMenu = GetActiveMenu();
		MenuBase *nextMenu = NULL;
		if (!gController.MenuStack.empty())
		{
			gController.MenuStack.pop();
		}
		if (!gController.MenuStack.empty())
		{
			nextMenu = gController.MenuStack.top();
		}
		FireActivationEvents(activeMenu, nextMenu);
	}
}

void MenuController_PopTo(MenuBase *menu, bool defer)
{
	if (defer)
	{
		gController.deferEvent.action = DeferAction_PopTo;
		gController.deferEvent.p1 = menu;
	}
	else
	{
		MenuBase *activeMenu = GetActiveMenu();
		MenuBase *nextMenu = NULL;
		while (!gController.MenuStack.empty())
		{
			if (gController.MenuStack.top() == menu)
			{
				nextMenu = menu;
				break;
			}
			gController.MenuStack.pop();
		}
		FireActivationEvents(activeMenu, nextMenu);
	}
}

void MenuController_Render(CPUTRenderParameters &renderParams)
{
	MenuBase *activeMenu = GetActiveMenu();
	if (activeMenu)
	{
		activeMenu->Render( renderParams );
	}
}
void MenuController_Update(float dt)
{
	if (gController.deferEvent.action != DeferAction_None)
	{
		MenuController_ExecuteDeferredEvent(&gController.deferEvent);
		gController.deferEvent.action = DeferAction_None;
	}

	MenuBase *activeMenu = GetActiveMenu();
	if (activeMenu)
	{
		activeMenu->Update(dt);
	}
}

void MenuController_Resize(int width, int height)
{
	MenuBase *activeMenu = GetActiveMenu();
	if (activeMenu)
	{
		activeMenu->Resize( width, height );
	}
}

void MenuController_HandleCPUTEvent(int eventId, int controlID, CPUTControl * control)
{
	MenuBase *activeMenu = GetActiveMenu();
	if (activeMenu)
	{
		activeMenu->HandleCPUTEvent(eventId, controlID, control);
	}
}


CPUTEventHandledCode MenuController_HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message)
{
	MenuBase *activeMenu = GetActiveMenu();
	if (activeMenu)
	{
		return activeMenu->HandleMouseEvent( x, y, wheel, state, message );
	}
	return CPUT_EVENT_UNHANDLED;
}

CPUTEventHandledCode MenuController_HandleKeyboardEvent(CPUTKey key, CPUTKeyState state)
{
	MenuBase *activeMenu = GetActiveMenu();
	if (activeMenu)
	{
		return activeMenu->HandleKeyboardEvent(key, state);
	}
	return CPUT_EVENT_UNHANDLED;
}