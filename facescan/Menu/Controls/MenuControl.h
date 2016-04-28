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