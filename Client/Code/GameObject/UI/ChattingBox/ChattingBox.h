#pragma once
#include "../UI.h"

enum class CHAT_STATE
{
	NONE = 0,
	CHATTING,
	CHAT_END
};

struct ChattingLog
{
	ChattingLog(const std::wstring& _chatting, int _x, int _y)
		: chatting(_chatting), x(_x), y(_y) {}

	std::wstring chatting;
	int x;
	int y;
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

	virtual bool CheckContain(int left, int top, int right, int bottom);

	void OpenChattingBox();

	void SetChatState(CHAT_STATE state) { mChatState = state; }
	void processInput();

	bool GetIsOpen()	const { return mOpen; }
	void SetCarrotPos();

private:
	bool mOpen;
	CHAT_STATE mChatState;

	std::vector<ChattingLog> mChattingLog;
	std::vector<wchar_t> mChattings;
	int mCarrotIndex;

	std::pair<float, float> mCarrotPos;
	float mElapsedTime;
};