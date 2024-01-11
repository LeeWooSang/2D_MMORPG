#include "TradeUI.h"
#include "../ButtonUI/ButtonUI.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../Resource/ResourceManager.h"
#include "../../../Resource/Texture/Texture.h"

#include "../../../Manager/SceneMangaer/SceneManager.h"
#include "../../../Scene/InGameScene/InGameScene.h"
#include "../Inventory/Inventory.h"

void CloseTradeUI(const std::string& name);
void CloseTradeUI(const std::string& name)
{
	TradeUI* ui = static_cast<TradeUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("TradeUI"));
	if (ui->GetIsOpen() == true)
	{
		ui->OpenTradeUI();
	}
}

TradeUI::TradeUI()
	: UI()
{
	mOpen = false;
}

TradeUI::~TradeUI()
{
}

bool TradeUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("TradeUIBackground0");
		SetTexture(data.name);
	}
	
	for (int i = 1; i <= 2; ++i)
	{
		std::string name = "TradeUIBackground" + std::to_string(i);
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(name);
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("Background", ui);
	}

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("TradeUIButton0");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		ui->SetLButtonClickCallback(CloseTradeUI);
		AddChildUI("Button", ui);
	}

	for (int i = 1; i < 4; ++i)
	{
		std::string name = "TradeUIButton" + std::to_string(i);
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(name);
		ButtonUI* ui = new ButtonUI;
		if(ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}	
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("Button", ui);
	}

	int slotNum = 0;
	int size = 32;
	int gapX = 7;
	int gapY = 5;
	for (int j = 0; j < 3; ++j)
	{
		for (int i = 0; i < 3; ++i)
		{
			TradeSlotUI* slot = new TradeSlotUI;
			int originX = 12 + (i * (size + gapX));
			int originY = 152 + (j * (size + gapY));
			if (slot->Initialize(originX, originY) == false)
			{
				return false;
			}
			slot->SetTexture("KeySlot");
			slot->SetSlotNum(slotNum++);
			slot->Visible();
			AddChildUI("Slot", slot);
		}
	}

	for (int j = 0; j < 3; ++j)
	{
		for (int i = 0; i < 3; ++i)
		{
			TradeSlotUI* slot = new TradeSlotUI;
			int originX = 150 + (i * (size + gapX));
			int originY = 152 + (j * (size + gapY));
			if (slot->Initialize(originX, originY) == false)
			{
				return false;
			}
			slot->SetTexture("KeySlot");
			slot->SetSlotNum(slotNum++);
			slot->Visible();
			AddChildUI("KeySlot", slot);
		}
	}

	//Visible();
	SetPosition(140, 150);

	return true;
}

void TradeUI::Update()
{
	UI::Update();
}

void TradeUI::Render()
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

void TradeUI::MouseOver()
{
}

void TradeUI::MouseLButtonDown()
{
}

void TradeUI::MouseLButtonUp()
{
}

void TradeUI::MouseLButtonClick()
{
	std::cout << "클릭" << std::endl;
}

void TradeUI::OpenTradeUI()
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

TradeSlotUI* TradeUI::FindSlot()
{
	std::vector<UI*>& v = FindChildUIs("Slot");
	for (int i = 0; i < v.size(); ++i)
	{
		TradeSlotUI* slot = static_cast<TradeSlotUI*>(v[i]);
		if (slot->GetMouseLButtonClick() == true)
		{
			std::cout << i << std::endl;
			return slot;
		}
	}

	return nullptr;
}

void TradeUI::Visible()
{
	mAttr |= ATTR_STATE_TYPE::VISIBLE;

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Visible();
		}
	}
}

void TradeUI::NotVisible()
{
	mAttr &= ~ATTR_STATE_TYPE::VISIBLE;

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->NotVisible();
		}
	}
}

TradeSlotUI::TradeSlotUI()
	: UI()
{
	mSlotNum = 0;
	mItem = nullptr;
}

TradeSlotUI::~TradeSlotUI()
{
	if (mItem != nullptr)
	{
		delete mItem;
		mItem = nullptr;
	}
}

bool TradeSlotUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	return true;
}

void TradeSlotUI::Update()
{
	UI::Update();
}

void TradeSlotUI::Render()
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

	//GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);
	//GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "검은색");

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

void TradeSlotUI::MouseOver()
{
}

void TradeSlotUI::MouseLButtonDown()
{
}

void TradeSlotUI::MouseLButtonUp()
{
}

void TradeSlotUI::MouseLButtonClick()
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

void TradeSlotUI::AddItem(InventoryItem* item)
{
	mItem = item;
	mItem->SetItemDrag(false);
	mItem->SetPosition(mPos.first, mPos.second);
}
