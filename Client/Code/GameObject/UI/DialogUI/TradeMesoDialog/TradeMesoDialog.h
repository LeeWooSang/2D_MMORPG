#pragma once
#include "../DialogUI.h"

class TradeMesoDialog : public DialogUI
{
public:
	TradeMesoDialog();
	virtual ~TradeMesoDialog();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

};

void OkButtonClick(const std::string& name);
void CancelButtonClick(const std::string& name);