#pragma once
#include "../UI.h"

class ChannelUI : public UI
{
public:
	ChannelUI();
	virtual ~ChannelUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	void OpenChannelUI();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

private:
	bool mOpen;
};

void ChannelClick(const std::string& channel);