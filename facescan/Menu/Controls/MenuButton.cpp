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