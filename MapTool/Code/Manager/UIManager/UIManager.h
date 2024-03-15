#pragma once
#include "../../Common/Macro.h"

class UI;
class UIManager
{
	SINGLE_TONE(UIManager)

public:
	void Render();
	void Update();

	void SetFocusUI(UI* ui);

private:
	UI* getTargetUI(UI* parentUI);
	UI* getFocusUI();

	UI* mFocusUI;
};