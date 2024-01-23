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

	const std::wstring GetText()	const { return mText; }
	void SetText(const std::wstring& text);
	void SetCarrotPos();

protected:
	virtual void processInput();

protected:
	std::wstring mText;
	float mElapsedTime;
	int mCarrotIndex;
	std::pair<float, float> mCarrotPos;
};