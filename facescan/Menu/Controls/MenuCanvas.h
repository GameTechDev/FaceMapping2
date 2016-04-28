#include "MenuControl.h"
#include <vector>

class MenuCanvas : public MenuControl
{
public:

	virtual void AddControl(MenuControl *control);
	virtual void RemoveControl(MenuControl *control);
	virtual void Render();
	virtual void Process(float dt);
	//virtual void ProcessInput(MouseParams input);
	
private:

	std::vector<MenuControl*> mControls;

};