#pragma once
#include "../UI.h"

class ExchangeUI : public UI
{
public:
	ExchangeUI();
	virtual ~ExchangeUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOverCollision(int x, int y);

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

private:

};

class ExchangeSlotUI : public UI
{
public:
	ExchangeSlotUI();
	virtual ~ExchangeSlotUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	int GetSlotNum()	const { return mSlotNum; }
	void SetSlotNum(int num) { mSlotNum = num; }

	void AddItem(int texId);
	void AddItem(const std::string& name);
	//void AddItem(InventoryItem* item);

private:
	int mSlotNum;
	//InventoryItem* mItem;
};
