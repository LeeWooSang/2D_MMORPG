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

	void AddItem(const std::string& name);
	InventoryItem* GetItem() { return mItem; }


private:
	int mSlotNum;
	InventoryItem* mItem;
};

constexpr int MAX_INVENTORY_WIDTH_SLOT_SIZE = 8;
constexpr int MAX_INVENTORY_HEIGHT_SLOT_SIZE = 4;
constexpr int VIEW_SLOT_HEIGHT = 5;

class Inventory : public UI
{
public:
	Inventory();
	virtual ~Inventory();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	void ProcessMouseWheelEvent(unsigned long long wParam);
	virtual bool CheckContain(int left, int top, int right, int bottom);

	void AddItem(const std::string& name);
	void AddItem(int slot, const std::string& name);

	void OpenInventory();

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

};