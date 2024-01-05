#pragma once
#include "../UI.h"

class ExchangeUI : public UI
{
public:
	ExchangeUI();
	virtual ~ExchangeUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOverCollision(int x, int y);

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

private:

};