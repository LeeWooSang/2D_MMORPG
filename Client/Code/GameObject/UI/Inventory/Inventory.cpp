#include "Inventory.h"
#include "../Scroll/Scroll.h"
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
			std::cout << "���� ��ȣ : " << mSlotNum << "Ŭ��" << std::endl;
		}
	}

	if (mItem != nullptr)
	{
		mItem->Update();
	}
}

void InventorySlot::Render()
{
	UI::Render();

	if (mItem != nullptr)
	{
		mItem->Render();
	}
}

void InventorySlot::AddItem(const std::string& name)
{
	// ������ �߰�
	mItem = new InventoryItem;
	mItem->SetPosition(mPos.first, mPos.second);
	// �ؽ��� �߰�
	mItem->SetTexture(name);
	mItem->Visible();
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

	int originX = 7;
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
			if (slot->SetTexture("Slot") == false)
			{
				return false;
			}

			// ���� ��ȣ
			slot->SetSlotNum(num++);
			slot->Visible();

			// �ڽ� �߰�
			AddChildUI("Slot", slot);

			tempX += (mSlotWidth + mSlotGap);
		}

		tempX = originX;
		tempY += (mSlotHeight + mSlotGap);
	}

	//if (mChildUIs["Slot"].size() > 20)
	//{
		Scroll* scroll = new Scroll;
		scroll->Initialize(300, 47);
		scroll->SetTexture("ScrollBackground");
		scroll->Visible();
		AddChildUI("Scroll", scroll);
	//}

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
		// ����ִ� ���Կ� �߰�
		if (slot->GetItem() == nullptr)
		{
			slot->AddItem(name);
			break;
		}
	}
}

void Inventory::AddItem(int slotNum, const std::string& name)
{
	InventorySlot* slot = static_cast<InventorySlot*>(mChildUIs["Slot"][slotNum]);
	if (slot->GetItem() == nullptr)
	{
		slot->AddItem(name);
	}
}

void Inventory::OpenInventory()
{
	// �����ִٸ�
	if (mOpen == false)
	{
		mOpen = true;
		Visible();		
	}
	// �����ִٸ�
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
