#pragma once
#include "../UI.h"

typedef void(*BTN_FUNC)	(const std::string&);

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

	void SetLButtonClickCallback(BTN_FUNC func, const std::string& name = "")
	{
		mFunc = func;
		mName = name;
	}

private:
	BTN_FUNC mFunc;
	std::string mName;
};