#pragma once
#include "../UI.h"
#include <array>

class InventorySlot : public UI
{
public:
	InventorySlot();
	virtual ~InventorySlot();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	void SetSlotNum(int num) { mSlotNum = num; }

private:
	int mSlotNum;
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

private:
	// 슬롯 간의 간격
	int mSlotGap;
	int mSlotWidth;
	int mSlotHeight;
};