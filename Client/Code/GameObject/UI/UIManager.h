#pragma once
#include "../../Common/Macro.h"

class UI;
class UIManager
{
	SINGLE_TONE(UIManager)

public:
	void Update();

private:
	UI* getTargetUI(UI* parentUI);
};