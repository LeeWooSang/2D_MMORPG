#include "ExchangeUI.h"
#include "../ButtonUI/ButtonUI.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../Resource/ResourceManager.h"
#include "../../../Resource/Texture/Texture.h"

#include "../../../Manager/SceneMangaer/SceneManager.h"
#include "../../../Scene/InGameScene/InGameScene.h"
#include "../Inventory/Inventory.h"

ExchangeUI::ExchangeUI()
	: UI()
{
}

ExchangeUI::~ExchangeUI()
{
}

bool ExchangeUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	SetTexture("ExchangeBackground0");

	int slotNum = 0;
	int size = 30;
	int gap = 10;
	for (int j = 0; j < 3; ++j)
	{
		for (int i = 0; i < 3; ++i)
		{
			ExchangeSlotUI* slot = new ExchangeSlotUI;
			int originX = 10 + (i * (size + gap));
			int originY = 180 + (j * (size + gap));
			if (slot->Initialize(originX, originY) == false)
			{
				return false;
			}
			slot->SetSlotNum(slotNum++);
			slot->Visible();
			AddChildUI("Slot", slot);
		}
	}

	for (int j = 0; j < 3; ++j)
	{
		for (int i = 0; i < 3; ++i)
		{
			ExchangeSlotUI* slot = new ExchangeSlotUI;
			int originX = 280 + (i * (size + gap));
			int originY = 180 + (j * (size + gap));
			if (slot->Initialize(originX, originY) == false)
			{
				return false;
			}
			slot->SetSlotNum(slotNum++);
			slot->Visible();
			AddChildUI("Slot", slot);
		}
	}

	{
		ButtonUI* ui = new ButtonUI;
		if(ui->Initialize(175, 40) == false)
		{
			return false;
		}
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("ExchangeButtonUI0");
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("Button", ui);
	}
	{
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(175, 75) == false)
		{
			return false;
		}
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("ExchangeButtonUI1");
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("Button", ui);
	}

	Visible();
	SetPosition(200, 200);

	return true;
}

void ExchangeUI::Update()
{
	UI::Update();
}

void ExchangeUI::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + mTexture->GetSize().first;
	pos.bottom = pos.top + mTexture->GetSize().second;
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

void ExchangeUI::MouseOver()
{
}

void ExchangeUI::MouseLButtonDown()
{
}

void ExchangeUI::MouseLButtonUp()
{
}

void ExchangeUI::MouseLButtonClick()
{
	std::cout << "click" << std::endl;
}

ExchangeSlotUI* ExchangeUI::FindSlot()
{
	std::vector<UI*>& v = FindChildUIs("Slot");
	for (int i = 0; i < v.size(); ++i)
	{
		ExchangeSlotUI* slot = static_cast<ExchangeSlotUI*>(v[i]);
		if (slot->GetMouseLButtonClick() == true)
		{
			std::cout << i << std::endl;
			return slot;
		}
	}

	return nullptr;
}

ExchangeSlotUI::ExchangeSlotUI()
	: UI()
{
	mSlotNum = 0;
	mItem = nullptr;
}

ExchangeSlotUI::~ExchangeSlotUI()
{
	if (mItem != nullptr)
	{
		delete mItem;
		mItem = nullptr;
	}
}

bool ExchangeSlotUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	SetTexture("InventorySlot");

	return true;
}

void ExchangeSlotUI::Update()
{
	UI::Update();
}

void ExchangeSlotUI::Render()
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
	pos.right = pos.left + mTexture->GetSize().first;
	pos.bottom = pos.top + mTexture->GetSize().second;

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

void ExchangeSlotUI::MouseOver()
{
}

void ExchangeSlotUI::MouseLButtonDown()
{
}

void ExchangeSlotUI::MouseLButtonUp()
{
}

void ExchangeSlotUI::MouseLButtonClick()
{
	Inventory* ui = static_cast<Inventory*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("Inventory"));
	InventorySlot* slot = ui->FindSlot();
	if (slot != nullptr)
	{
		AddItem(slot->GetItem());
		slot->SetItem();
		slot->SetMouseLButtonClick(false);
	}
}

void ExchangeSlotUI::AddItem(InventoryItem* item)
{
	mItem = item;
	mItem->SetItemDrag(false);
	mItem->SetPosition(mPos.first, mPos.second);
}
