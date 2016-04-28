#ifndef __MENU_TYPES__
#define __MENU_TYPES__

struct MenuInputEvent
{

};

struct MenuPointF
{
	MenuPointF(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	MenuPointF()
	{

	}


	float x;
	float y;
};

struct MenuSizeF
{
	float width;
	float height;
};

struct MenuRectF
{
	float x;
	float y;
	float width;
	float height;
};

#endif