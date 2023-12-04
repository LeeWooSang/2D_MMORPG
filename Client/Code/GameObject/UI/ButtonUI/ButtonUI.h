#pragma once
#include "../UI.h"

class ButtonUI : public UI
{
public:
	ButtonUI();
	virtual ~ButtonUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();
};