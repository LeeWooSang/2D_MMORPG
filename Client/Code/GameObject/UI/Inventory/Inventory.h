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

	void SetSlotNum(int num) { mSlotNum = num; }

	void AddItem(const std::string& name);
	InventoryItem* GetItem() { return mItem; }


private:
	int mSlotNum;
	InventoryItem* mItem;
};

constexpr int MAX_INVENTORY_WIDTH_SLOT_SIZE = 5;
constexpr int MAX_INVENTORY_HEIGHT_SLOT_SIZE = 4;
class Inventory : public UI
{
public:
	Inventory();
	virtual ~Inventory();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	void AddItem(const std::string& name);
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