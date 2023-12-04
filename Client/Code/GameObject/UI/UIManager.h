#pragma once
#include "../../Common/Macro.h"
#include <list>
#include <vector>
#include <unordered_map>
#include <string>

class UI;
class UIManager
{
	SINGLE_TONE(UIManager)

public:
	void Render();
	void Update();

	UI* FindUI(const std::string& name);
	void AddUI(const std::string& name, UI* ui);
	void SetFocusUI(UI* ui);

private:
	std::unordered_map<std::string, UI*> mUIsMap;
	std::list<UI*> mUIs;
	//std::vector<UI*> mUIs;

	UI* getTargetUI(UI* parentUI);
	UI* getFocusUI();

	UI* mFocusUI;
};