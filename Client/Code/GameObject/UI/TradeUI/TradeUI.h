#pragma once
#include "../UI.h"

class TradeUI : public UI
{
public:
	TradeUI();
	virtual ~TradeUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	class TradeSlotUI* FindSlot();

private:

};

class InventoryItem;
class TradeSlotUI : public UI
{
public:
	TradeSlotUI();
	virtual ~TradeSlotUI();
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
	void AddItem(InventoryItem* item);

private:
	int mSlotNum;
	InventoryItem* mItem;
};
