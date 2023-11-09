#include "Inventory.h"
#include "../../../Resource/Texture/Texture.h"
#include "../../../Input/Input.h"

InventorySlot::InventorySlot()
	: UI()
{
	mSlotNum = 0;
	mItem = nullptr;
}

InventorySlot::~InventorySlot()
{
	if (mItem != nullptr)
	{
		delete mItem;
	}
}

bool InventorySlot::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	return true;
}

void InventorySlot::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	if (Collision(mousePos.first, mousePos.second) == true)
	{
		if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::MOUSE_LBUTTON) == true)
		{
			std::cout << "슬롯 번호 : " << mSlotNum << "클릭" << std::endl;
		}
	}
}

void InventorySlot::Render()
{
	UI::Render();
}

void InventorySlot::AddItem(const std::string& name)
{
	// 아이템 추가
	mItem = new InventoryItem;
	mItem->SetPosition(mPos.first, mPos.second);
	// 텍스쳐 추가
	SetTexture(name);
}

Inventory::Inventory()
	: UI()
{
	mSlotGap = 0;
	mSlotWidth = 0;
	mSlotHeight = 0;

	mOpen = false;
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

	int originX = 8;
	int originY = 47;
	int tempX = originX;
	int tempY = originY;

	int num = 0;
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

			// 슬롯 번호
			slot->SetSlotNum(num++);
			slot->Visible();

			// 자식 추가
			AddChildUI("Slot", slot);

			tempX += (mSlotWidth + mSlotGap);
		}

		tempX = originX;
		tempY += (mSlotHeight + mSlotGap);
	}

	SetPosition(x, y);

	return true;
}

void Inventory::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Update();
		}
	}
}

void Inventory::Render()
{
	UI::Render();
}

void Inventory::AddItem(const std::string& name)
{
	for (auto& inventorySlot : mChildUIs["Slot"])
	{
		InventorySlot* slot = static_cast<InventorySlot*>(inventorySlot);
		// 비어있는 슬롯에 추가
		if (slot->GetItem() == nullptr)
		{
			slot->AddItem(name);
			break;
		}
	}
}

void Inventory::OpenInventory()
{
	// 닫혀있다면
	if (mOpen == false)
	{
		mOpen = true;
		Visible();		
	}
	// 열려있다면
	else
	{
		mOpen = false;
		NotVisible();		
	}
}

InventoryItem::InventoryItem()
	: UI()
{
}

InventoryItem::~InventoryItem()
{
}

bool InventoryItem::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	return true;
}

void InventoryItem::Update()
{
	UI::Update();
}

void InventoryItem::Render()
{
	UI::Render();
}
