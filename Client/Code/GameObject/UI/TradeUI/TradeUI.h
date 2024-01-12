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

	void OpenTradeUI();

	class TradeSlotUI* FindSlot();

	bool GetIsOpen()	const { return mOpen; }

	int GetTradeUserId()	const { return mTradeUserId; }
	void SetTradeUserId(int id) { mTradeUserId = id; }
	void ResetTradeUserId() { mTradeUserId = -1; }

	void TradePostProcessing();

	virtual void Visible();
	virtual void NotVisible();

private:
	bool mOpen;
	int mTradeUserId;
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

	void AddItem(InventoryItem* item);
	InventoryItem* GetItem() { return mItem; }
	void ResetSlot();

private:
	int mSlotNum;
	InventoryItem* mItem;
};

void CloseTradeUI(const std::string& name);