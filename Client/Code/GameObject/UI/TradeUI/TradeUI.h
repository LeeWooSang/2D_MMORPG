#pragma once
#include "../UI.h"

class DialogUI;
constexpr int MAX_TRADE_SLOT = 9;
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

	void AddItemOfTradeUser(int texId, int slotNum);

	void TradePostProcessing();
	void ProcessTradeCancel();

	virtual void Visible();
	virtual void NotVisible();

	long long GetMyMeso()	const { return mMyMeso; }
	void SetMyMeso(long long meso) { mMyMeso = meso; }
	void SetTradeUserMeso(long long meso) { mTradeUserMeso = meso; }
	
	void AddMyMeso(long long meso) { mMyMeso += meso; }
	void AddTradeUserMeso(long long meso) { mTradeUserMeso += meso; }

	DialogUI* GetMesoDialogUI() { return mMesoDialogUI; }

private:
	bool mOpen;
	int mTradeUserId;

	long long mMyMeso;
	long long mTradeUserMeso;

	DialogUI* mMesoDialogUI;
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
	// 다른 사람이 아이템을 올릴 때
	void AddItem(int texId);

	InventoryItem* GetItem() { return mItem; }
	void SetItem() { mItem = nullptr; }

	void ResetSlot();

private:
	int mSlotNum;
	InventoryItem* mItem;
};

void TradeCancelClick(const std::string& name);
void TradeClick(const std::string& name);
void AddMesoClick(const std::string& name);