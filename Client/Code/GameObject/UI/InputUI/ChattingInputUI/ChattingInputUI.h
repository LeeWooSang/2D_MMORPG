#pragma once
#include "../InputUI.h"
#include "../../ChattingBox/ChattingBox.h"

enum class CHAT_STATE
{
	NONE = 0,
	CHATTING,
	CHAT_END
};

class ChattingInputUI : public InputUI
{
public:
	ChattingInputUI();
	virtual ~ChattingInputUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	void OpenChattingBox();
	bool GetIsOpen()	const { return mOpen; }

private:
	virtual void processInput();

private:
	bool mOpen;
	CHAT_STATE mChatState;

	bool mWhispering;
};