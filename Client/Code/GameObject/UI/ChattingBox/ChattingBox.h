#pragma once
#include "../UI.h"

class ChattingBox : public UI
{
public:
	ChattingBox();
	virtual ~ChattingBox();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOverCollision(int x, int y);

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	void OpenChattingBox();
	void processInput();

	bool GetIsOpen()	const { return mOpen; }
	std::pair<float, float> GetTextSize(wchar_t c);

private:
	bool mOpen;
	std::vector<wchar_t> mChattings;
	int mCarrotIndex;

	std::pair<float, float> mCarrotPos;
	float mElapsedTime;
};