#pragma once
#include "../UI.h"
#include <mutex>

enum class CHAT_STATE
{
	NONE = 0,
	CHATTING,
	CHAT_END
};

struct ChattingLog
{
	ChattingLog(int _id, const std::wstring& _chatting, int _x, int _y)
		: id(_id), chatting(_chatting), x(_x), y(_y) {}

	int id;
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
	void AddChattingLog(int id, wchar_t* chatting);

	CHAT_STATE GetChatState()		const { return mChatState; }
	void SetChatState(CHAT_STATE state) { mChatState = state; }

	bool GetIsOpen()	const { return mOpen; }
	void SetCarrotPos();

private:
	void processInput();

private:
	bool mOpen;
	float mElapsedTime;

	CHAT_STATE mChatState;

	std::vector<ChattingLog> mChattingLog;
	std::vector<wchar_t> mChattings;
	int mCarrotIndex;
	std::pair<float, float> mCarrotPos;

	std::mutex mChattingLogMtx;
};