#pragma once
#include "../UI.h"

class Scroll : public UI
{
public:
	Scroll();
	virtual ~Scroll();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	int GetAlphaValue()	const { return mAlphaValue; }
	void SetAlphaValue(int value);

private:
	int mAlphaValue;
};

class ScrollBar : public UI
{
public:
	ScrollBar();
	virtual ~ScrollBar();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	virtual void MouseOverCollision(int x, int y);

	int GetAlphaValue()	const { return mAlphaValue; }
	void SetAlphaValue(int value) { mAlphaValue = value; }

private:
	int mAlphaValue;
};