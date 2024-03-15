#pragma once
#include "../UI.h"

class GameMenuUI : public UI
{
public:
	GameMenuUI();
	virtual ~GameMenuUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	void OpenGameMenuUI();

protected:
	bool mOpen;
};

class InGameMenuUI : public GameMenuUI
{
public:
	InGameMenuUI();
	virtual ~InGameMenuUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();
};

void ChannelChangeMenuClick(const std::string& name);
void GameOptionMenuClick(const std::string& name);