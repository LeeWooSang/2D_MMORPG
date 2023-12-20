#pragma once
#include "../UI.h"
#include <array>

class InventoryItem;
class InventorySlot : public UI
{
public:
	InventorySlot();
	virtual ~InventorySlot();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	virtual void SetPosition(int x, int y);

	int GetSlotNum()	const { return mSlotNum; }
	void SetSlotNum(int num) { mSlotNum = num; }

	void AddItem(int texId);
	void AddItem(const std::string& name);
	void AddItem(InventoryItem* item);

	InventoryItem* GetItem() { return mItem; }


private:
	int mSlotNum;
	InventoryItem* mItem;
};

constexpr int MAX_INVENTORY_WIDTH_SLOT_SIZE = 6;
constexpr int MAX_INVENTORY_HEIGHT_SLOT_SIZE = 4;
constexpr int VIEW_SLOT_HEIGHT = 6;

class Inventory : public UI
{
public:
	Inventory();
	virtual ~Inventory();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	void ProcessMouseWheelEvent(unsigned long long wParam);
	virtual bool CheckContain(int left, int top, int right, int bottom);

	void AddItem(int texId);
	void AddItem(const std::string& name);
	void AddItem(int slot, const std::string& name);
	void AddItem(InventoryItem* item);

	void OpenInventory();

	std::pair<int, int> GetDragStartPos()	const { return mDragStartPos; }
	void SetDragStartPos(std::pair<int, int> pos) { mDragStartPos = pos; }

private:
	// 슬롯 간의 간격
	int mSlotGap;
	int mSlotWidth;
	int mSlotHeight;

	bool mOpen;
};

class InventoryItem : public UI
{
public:
	InventoryItem();
	virtual ~InventoryItem();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	const std::string& GetItemName()	const { return mItemName; }
	void SetItemName(const std::string& name) { mItemName = name; }

	int GetTexId()	const { return mTexId; }
	void SetTexId(int id) { mTexId = id; }

private:
	std::string mItemName;
	int mTexId;
};