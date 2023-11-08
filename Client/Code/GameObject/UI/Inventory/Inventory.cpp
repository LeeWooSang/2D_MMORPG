#include "Inventory.h"

InventorySlot::InventorySlot()
	: UI()
{
}

InventorySlot::~InventorySlot()
{
}

bool InventorySlot::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	return true;
}

void InventorySlot::Update()
{
	UI::Update();
}

void InventorySlot::Render()
{
	UI::Render();
}

Inventory::Inventory()
	: UI()
{
	mSlotGap = 0;
}

Inventory::~Inventory()
{
}

bool Inventory::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	mSlotGap = 10;
	mSlotWidth = 64;
	mSlotHeight = 64;

	int originX = 9;
	int originY = 47;
	int tempX = originX;
	int tempY = originY;

	for (int i = 0; i < MAX_INVENTORY_WIDTH_SLOT_SIZE; ++i)
	{
		for (int j = 0; j < MAX_INVENTORY_HEIGHT_SLOT_SIZE; ++j)
		{
			InventorySlot* slot = new InventorySlot;
			if (slot->Initialize(tempX, tempY) == false)
			{
				return false;
			}
			if (slot->SetTexture("Sword") == false)
			{
				return false;
			}
			slot->Visible();

			// 자식 추가
			AddChildUI(slot);

			tempX += (mSlotWidth + mSlotGap);
		}

		tempX = originX;
		tempY += (mSlotHeight + mSlotGap);
	}

	return true;
}

void Inventory::Update()
{
	UI::Update();
}

void Inventory::Render()
{
	UI::Render();
}
