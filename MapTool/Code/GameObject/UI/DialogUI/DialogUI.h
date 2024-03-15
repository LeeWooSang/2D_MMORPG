#pragma once
#include "../UI.h"

class DialogUI : public UI
{
public:
	DialogUI();
	virtual ~DialogUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	void OpenDialogUI();

	virtual void Visible();
	virtual void NotVisible();

	bool GetIsOpen()	const { return mOpen; }
private:
	bool mOpen;
};