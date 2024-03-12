#include "EquipUI.h"
#include "../../../Network/Network.h"

#include "../ButtonUI/ButtonUI.h"
#include "../../../Manager/SceneMangaer/SceneManager.h"
//#include "../../../Scene/Scene.h"
#include "../../../Scene/InGameScene/InGameScene.h"

#include "../../../Resource/ResourceManager.h"
#include "../../../Resource/Texture/Texture.h"

#include "../../../Input/Input.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../Inventory/Inventory.h"
#include "../../Character/Character.h"
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

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("EquipUIBackground0");
		SetTexture(data.name);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("EquipUIBackground1");
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("EquipUIBackground2");
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("EquipUIBackground3");
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("EquipUIBackground4");
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("EquipUITab00");
		UI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("EquipUITab11");
		UI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("EquipUIButton0");
		UI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("EquipUIButton1");
		UI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI(data.name, ui);
	}

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("EquipUISlot0");
		for (int i = 0; i < 4; ++i)
		{
			std::pair<int, int> origin = data.origin;
			origin.second += (41 * i);

			EquipSlotUI* slotUI = new EquipSlotUI;
			if (slotUI->Initialize(origin.first, origin.second) == false)
			{
				return false;
			}
			slotUI->SetTexture(data.name);
			slotUI->SetSlotType(EQUIP_SLOT_TYPE::RING);
			slotUI->Visible();
			//AddChildUI("Slot", slotUI);
			AddChildUI(data.name, slotUI);
		}
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("EquipUISlot1");
		EquipSlotUI* slotUI = new EquipSlotUI;
		if (slotUI->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		slotUI->SetTexture(data.name);
		slotUI->SetSlotType(EQUIP_SLOT_TYPE::POCKET);
		slotUI->Visible();
		AddChildUI("Slot", slotUI);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("EquipUISlot2");
		for (int i = 0; i < 2; ++i)
		{
			std::pair<int, int> origin = data.origin;
			origin.second += (41 * i);

			EquipSlotUI* slotUI = new EquipSlotUI;
			if (slotUI->Initialize(origin.first, origin.second) == false)
			{
				return false;
			}
			slotUI->SetTexture(data.name);
			slotUI->SetSlotType(EQUIP_SLOT_TYPE::PENDANT);
			slotUI->Visible();
			AddChildUI("Slot", slotUI);
		}
	}
	for (int i = static_cast<int>(EQUIP_SLOT_TYPE::WEAPON); i <= static_cast<int>(EQUIP_SLOT_TYPE::HEART); ++i)
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("EquipUISlot" + std::to_string(i));
		EquipSlotUI* slotUI = new EquipSlotUI;
		if (slotUI->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		slotUI->SetTexture(data.name);
		slotUI->SetSlotType(static_cast<EQUIP_SLOT_TYPE>(i));
		slotUI->Visible();
		AddChildUI("Slot", slotUI);
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
	if (mMouseLButtonDown == true)
	{
		Move(mousePos.first, mousePos.second);
	}

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
	pos.right = pos.left + mRect.first;
	pos.bottom = pos.top + mRect.second;

	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

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

void EquipUI::MouseOver()
{
}

void EquipUI::MouseLButtonDown()
{
	UI::MouseLButtonDown();
}

void EquipUI::MouseLButtonUp()
{
	UI::MouseLButtonUp();
}

void EquipUI::MouseLButtonClick()
{
	UI::MouseLButtonClick();
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

void EquipUI::AddEquipItem(int slot, int texId)
{
	TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureDataIcon(texId);
	InventoryItem* item = new InventoryItem;
	// 아이템 추가
	item->SetTexId(data.texId);
	item->SetItemName(data.name);
	item->SetItemType(data.equipSlotType);

	// 텍스쳐 추가
	item->SetTexture(data.name);
	item->Visible();

	EquipSlotUI* slotUI = static_cast<EquipSlotUI*>(FindChildUIs("Slot")[slot]);
	slotUI->AddItem(item);
}

InventoryItem* EquipUI::AddEquipItem(InventoryItem* item)
{
	EquipSlotUI* slotUI = static_cast<EquipSlotUI*>(FindChildUIs("Slot")[item->GetItemType()]);
	return slotUI->AddItem(item);
}

EquipSlotUI::EquipSlotUI()
	: UI()
{
	mType = EQUIP_SLOT_TYPE::CAP;
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

	if (mItem != nullptr)
	{
		mItem->Update();
	}
}

void EquipSlotUI::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + mRect.first;
	pos.bottom = pos.top + mRect.second;

	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	if (mItem != nullptr)
	{
		mItem->Render();
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
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

	InGameScene* scene = static_cast<InGameScene*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE));
	Inventory* inventory = static_cast<Inventory*>(scene->FindUI("Inventory"));
	inventory->AddItem(mItem);
	mItem = nullptr;
	
	// 아바타 변경
	//scene->GetPlayer()->SetAvatar();
#ifdef SERVER_CONNECT
	//GET_INSTANCE(Network)->SendTakeOffEquipItem(static_cast<char>(mType));	
#endif // SERVER_CONNECT
}

void EquipSlotUI::SetPosition(int x, int y)
{
	UI::SetPosition(x, y);

	if (mItem != nullptr)
	{
		mItem->SetPosition(mPos.first + 5, mPos.second + 5);
	}
}

InventoryItem* EquipSlotUI::AddItem(InventoryItem* newItem)
{
	// 기존 아이템 저장
	InventoryItem* oldItem = mItem;

	// 새로운 아이템 저장
	mItem = newItem;
	mItem->SetPosition(mPos.first + 5, mPos.second + 5);

	return oldItem;
}
