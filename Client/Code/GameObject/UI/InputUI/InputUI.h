#pragma once
#include "../UI.h"
#include <string>

class InputUI : public UI
{
public:
	InputUI();
	virtual ~InputUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	void SetText(const std::wstring& text);

protected:
	virtual void processInput();
	void setCarrotPos();

protected:
	std::wstring mText;
	float mElapsedTime;
	int mCarrotIndex;
	std::pair<float, float> mCarrotPos;
};