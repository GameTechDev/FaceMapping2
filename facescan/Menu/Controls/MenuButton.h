#ifndef __MENU_BUTTON__
#define __MENU_BUTTON__

#include "MenuControl.h"
#include <string>

class MenuButton : public MenuControl
{
public:
	MenuButton();

	void SetText(std::string text) { mText = text; }
	virtual void Render();


private:

	std::string mText;
};

#endif