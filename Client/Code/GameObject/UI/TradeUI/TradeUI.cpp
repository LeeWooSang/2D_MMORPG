#include "TradeUI.h"
#include "../../../Network/Network.h"

#include "../ButtonUI/ButtonUI.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../Resource/ResourceManager.h"
#include "../../../Resource/Texture/Texture.h"

#include "../../../Manager/SceneMangaer/SceneManager.h"
#include "../../../Scene/InGameScene/InGameScene.h"
#include "../Inventory/Inventory.h"

void TradeCancelClick(const std::string& name)
{
	TradeUI* trade = static_cast<TradeUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("TradeUI"));

#ifdef SERVER_CONNECT
	// ��� ��ư Ŭ�� ����
	GET_INSTANCE(Network)->SendTradeCancelPacket(trade->GetTradeUserId());
#endif // SERVER_CONNECT

	trade->ProcessTradeCancel();
}

void TradeClick(const std::string& name)
{
	TradeUI* ui = static_cast<TradeUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("TradeUI"));
	if (ui->GetIsOpen() == false)
	{
		return;
	}

	// ���� �ø� ������ ���̵� ������ �迭
	int items[9];
	memset(items, -1, sizeof(items));

	std::vector<UI*>& v = ui->FindChildUIs("Slot");
	for (int i = 0; i < v.size(); ++i)
	{
		TradeSlotUI* slot = static_cast<TradeSlotUI*>(v[i]);
		if (slot->GetItem() == nullptr)
		{
			continue;
		}
		items[i] = slot->GetItem()->GetTexId();
	}

#ifdef SERVER_CONNECT
	GET_INSTANCE(Network)->SendTradePacket(ui->GetTradeUserId(), items);
#endif // SERVER_CONNECT
}

TradeUI::TradeUI()
	: UI()
{
	mOpen = false;
	mTradeUserId = -1;
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
		ui->SetLButtonClickCallback(TradeCancelClick);
		AddChildUI("Button", ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("TradeUIButton1");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		ui->SetLButtonClickCallback(TradeClick);
		AddChildUI("Button", ui);
	}

	for (int i = 2; i < 4; ++i)
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
			int originX = 150 + (i * (size + gapX));
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
	// ���̴� �͸� ����
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
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "�Ķ���");
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
	std::cout << "Ŭ��" << std::endl;
}

void TradeUI::OpenTradeUI()
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

TradeSlotUI* TradeUI::FindSlot()
{
	std::vector<UI*>& v = FindChildUIs("Slot");
	for (int i = 0; i < v.size(); ++i)
	{
		TradeSlotUI* slot = static_cast<TradeSlotUI*>(v[i]);
		if (slot->GetMouseLButtonClick() == true)
		{
			return slot;
		}
	}

	return nullptr;
}

void TradeUI::AddItemOfTradeUser(int texId, int slotNum)
{
	// ���� ������ ���� ���Թ�ȣ + 9
	int index = slotNum + 9;
	std::vector<UI*>& v = FindChildUIs("Slot");
	TradeSlotUI* slot = static_cast<TradeSlotUI*>(v[index]);
	slot->AddItem(texId);
}

void TradeUI::TradePostProcessing()
{
	std::vector<UI*>& v = FindChildUIs("Slot");
	for (int i = 0; i < v.size(); ++i)
	{
		TradeSlotUI* slot = static_cast<TradeSlotUI*>(v[i]);
		slot->ResetSlot();
	}

	if (mOpen == false)
	{
		return;
	}
	OpenTradeUI();
	ResetTradeUserId();
}

void TradeUI::ProcessTradeCancel()
{
	TradeUI* trade = static_cast<TradeUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("TradeUI"));
	if (trade->GetIsOpen() == false)
	{
		return;
	}
	std::vector<UI*>& v = trade->FindChildUIs("Slot");

	// �� �������� �κ��丮�� ����ġ ���Ѿ���
	Inventory* inventory = static_cast<Inventory*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("Inventory"));
	for (int i = 0; i < 9; ++i)
	{
		TradeSlotUI* slot = static_cast<TradeSlotUI*>(v[i]);
		if (slot->GetItem() == nullptr)
		{
			continue;
		}
		inventory->AddItem(slot->GetItem());
		// ������ ������ null�� �ʱ�ȭ
		slot->SetItem();
	}

	// ���� ��ȯ ����
	for (int i = 9; i < v.size(); ++i)
	{
		static_cast<TradeSlotUI*>(v[i])->ResetSlot();
	}

	trade->OpenTradeUI();
	trade->ResetTradeUserId();
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
	// ���̴� �͸� ����
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
	//GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "������");

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "�Ķ���");
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

#ifdef SERVER_CONNECT
	int userId = static_cast<TradeUI*>(mParentUI)->GetTradeUserId();
	// �������� �÷ȴٰ� ��Ŷ ����
	GET_INSTANCE(Network)->SendAddTradeItem(userId, mItem->GetTexId(), mSlotNum);
#endif // SERVER_CONNECT
}

void TradeSlotUI::AddItem(int texId)
{
	TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureDataIcon(texId);

	// ������ �߰�
	mItem = new InventoryItem;
	if (mItem->Initialize(0, 0) == false)
	{
		return;
	}
	mItem->SetTexId(data.texId);
	mItem->SetItemName(data.name);
	mItem->SetItemType(data.equipSlotType);

	// �ؽ��� �߰�
	mItem->SetTexture(data.name);
	mItem->Visible();
	mItem->SetPosition(mPos.first, mPos.second);
}

void TradeSlotUI::ResetSlot()
{
	if (mItem != nullptr)
	{
		delete mItem;
		mItem = nullptr;
	}
}