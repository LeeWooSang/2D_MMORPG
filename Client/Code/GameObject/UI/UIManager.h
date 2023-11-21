#pragma once
#include "../../Common/Macro.h"
#include <list>
#include <vector>

class UI;
class UIManager
{
	SINGLE_TONE(UIManager)

public:
	void Render();
	void Update();
	void AddUI(UI* ui) { mUIs.emplace_back(ui); };
	void SetFocusUI(UI* ui);

private:
	std::list<UI*> mUIs;
	//std::vector<UI*> mUIs;

	UI* getTargetUI(UI* parentUI);
	UI* getFocusUI();

	UI* mFocusUI;
};