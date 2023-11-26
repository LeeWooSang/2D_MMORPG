#pragma once
#include "../UI.h"

enum class CHAT_STATE
{
	NONE = 0,
	CHATTING,
	CHAT_END
};

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

	void SetChatState(CHAT_STATE state) { mChatState = state; }
	void processInput();

	bool GetIsOpen()	const { return mOpen; }
	void SetCarrotPos();

private:
	bool mOpen;
	CHAT_STATE mChatState;

	std::vector<wchar_t> mChattings;
	int mCarrotIndex;

	std::pair<float, float> mCarrotPos;
	float mElapsedTime;
};