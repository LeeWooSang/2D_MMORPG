#include "EquipUI.h"
#include "../../../Scene/SceneManager.h"
#include "../../../Scene/Scene.h"

#include "../../../Input/Input.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../Inventory/Inventory.h"
#include <vector>

EquipUI::EquipUI()
	: UI()
{
	mOpen = false;
}

EquipUI::~EquipUI()
{
}

bool EquipUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	std::vector<std::pair<std::string, EQUIP_SLOT_TYPE>> v
	{ 
		{"head", EQUIP_SLOT_TYPE::HEAD},
		{"topBody", EQUIP_SLOT_TYPE::TOP_BODY},
		{"bottomBody", EQUIP_SLOT_TYPE::BOTTOM_BODY},
		{"hand", EQUIP_SLOT_TYPE::HAND},
		{"weapon", EQUIP_SLOT_TYPE::WEAPON}
	};

	for (int i = 0; i <= static_cast<int>(EQUIP_SLOT_TYPE::WEAPON); ++i)
	{
		EquipSlotUI* slotUI = new EquipSlotUI;
		if (slotUI->Initialize(i * 75, 10) == false)
		{
			return false;
		}
		slotUI->SetSlotType(v[i].second);
		slotUI->Visible();
		AddChildUI(v[i].first, slotUI);
	}

	SetPosition(100, 100);

	return true;
}

void EquipUI::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	MouseOverCollision(mousePos.first, mousePos.second);

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Update();
		}
	}
}

void EquipUI::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	//pos.right = pos.left + mTexture->GetSize().first;
	//pos.bottom = pos.top + mTexture->GetSize().second;
	pos.right = pos.left + 400;
	pos.bottom = pos.top + 400;

	GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "흰색");
	//GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}

void EquipUI::MouseOverCollision(int x, int y)
{
	// ui의 rect와 닿았는가?
	RECT collisionBox;
	collisionBox.left = mPos.first;
	collisionBox.right = mPos.first + 400;
	collisionBox.top = mPos.second;
	collisionBox.bottom = mPos.second + 400;

	if (x >= collisionBox.left && x <= collisionBox.right && y >= collisionBox.top && y <= collisionBox.bottom)
	{
		mMouseOver = true;
	}
	else
	{
		mMouseOver = false;
	}
}

void EquipUI::MouseOver()
{
}

void EquipUI::MouseLButtonDown()
{
}

void EquipUI::MouseLButtonUp()
{
}

void EquipUI::MouseLButtonClick()
{
}

void EquipUI::OpenEquipUI()
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

InventoryItem* EquipUI::AddEquipItem(const std::string& key, InventoryItem* item)
{
	EquipSlotUI* slotUI = static_cast<EquipSlotUI*>(FindChildUIs(key).front());
	return slotUI->AddItem(item);
}

EquipSlotUI::EquipSlotUI()
	: UI()
{
	mType = EQUIP_SLOT_TYPE::HEAD;
	mItem = nullptr;
}

EquipSlotUI::~EquipSlotUI()
{
	if (mItem != nullptr)
	{
		delete mItem;
		mItem = nullptr;
	}
}

bool EquipSlotUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);
	
	return true;
}

void EquipSlotUI::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	MouseOverCollision(mousePos.first, mousePos.second);

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Update();
		}
	}
}

void EquipSlotUI::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	if (mItem != nullptr)
	{
		mItem->Render();
		return;
	}

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	//pos.right = pos.left + mTexture->GetSize().first;
	//pos.bottom = pos.top + mTexture->GetSize().second;
	pos.right = pos.left + 65;
	pos.bottom = pos.top + 65;

	GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "검은색");
	//GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}

void EquipSlotUI::MouseOverCollision(int x, int y)
{
	// ui의 rect와 닿았는가?
	RECT collisionBox;
	collisionBox.left = mPos.first;
	collisionBox.right = mPos.first + 65;
	collisionBox.top = mPos.second;
	collisionBox.bottom = mPos.second + 65;

	if (x >= collisionBox.left && x <= collisionBox.right && y >= collisionBox.top && y <= collisionBox.bottom)
	{
		mMouseOver = true;
	}
	else
	{
		mMouseOver = false;
	}
}

void EquipSlotUI::MouseOver()
{
}

void EquipSlotUI::MouseLButtonDown()
{
}

void EquipSlotUI::MouseLButtonUp()
{
}

void EquipSlotUI::MouseLButtonClick()
{
	if (mItem == nullptr)
	{
		return;
	}

	Inventory* inventory = static_cast<Inventory*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("Inventory"));
	inventory->AddItem(mItem);
	mItem = nullptr;
}

InventoryItem* EquipSlotUI::AddItem(InventoryItem* newItem)
{
	// 기존 아이템 저장
	InventoryItem* oldItem = mItem;

	// 새로운 아이템 저장
	mItem = newItem;
	mItem->SetPosition(mPos.first, mPos.second);

	return oldItem;
}
