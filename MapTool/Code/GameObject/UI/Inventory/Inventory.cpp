#include "Inventory.h"
#include "../../../Network/Network.h"
#include "../../../Manager/SceneMangaer/SceneManager.h"
#include "../../../Scene/Scene.h"
#include "../../../Scene/InGameScene/InGameScene.h"

#include "../../../GraphicEngine/GraphicEngine.h"

#include "../Scroll/Scroll.h"
#include "../ButtonUI/ButtonUI.h"
#include "../../../Resource/ResourceManager.h"
#include "../../../Resource/Texture/Texture.h"
#include "../../../Input/Input.h"

#include "../EquipUI/EquipUI.h"
#include "../../Character/Character.h"

void CloseInventory(const std::string& name);
void CloseInventory(const std::string& skillName)
{
	Inventory* ui = static_cast<Inventory*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("Inventory"));
	ui->OpenInventory();

	//if (ui->GetIsOpen() == true)
	//{
	//	ui->OpenInventory();
	//}
}

InventorySlot::InventorySlot()
	: UI()
{
	mSlotNum = 0;
	mItem = nullptr;
	mMouseLButtonDoubleClick = false;
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
	MouseOverCollision(mousePos.first, mousePos.second);

	if (mItem != nullptr)
	{
		mItem->Update();
	}
}

void InventorySlot::Render()
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
	pos.right = pos.left + mRect.first;
	pos.bottom = pos.top + mRect.second;

	//GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);
	GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "������");

	//std::wstring text = std::to_wstring(mSlotNum);
	//GET_INSTANCE(GraphicEngine)->RenderText(text, mPos.first, mPos.second, "������", "�ϴû�");

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

void InventorySlot::MouseOver()
{
	UI::MouseOver();
}

void InventorySlot::MouseLButtonDown()
{
	UI::MouseLButtonDown();

	if (mItem != nullptr)
	{
		mItem->MouseLButtonDown();
	}
}

void InventorySlot::MouseLButtonUp()
{
	UI::MouseLButtonUp();
}

void InventorySlot::MouseLButtonClick()
{
	if (mMouseLButtonDoubleClick == true)
	{
		std::cout << "���� Ŭ��" << std::endl;

		mMouseLButtonClick = false;

		// �κ��丮���� ���õ� �������� ���� �� ó��
		Inventory* inven = static_cast<Inventory*>(mParentUI);
		if (inven->GetPickingItem() != nullptr)
		{
			inven->GetPickingItem()->SetItemDrag(false);
			inven->ResetPicking();
		}

		MouseLButtonDoubleClick();
		mMouseLButtonDoubleClick = false;
	}
	else
	{
		std::cout << "�� Ŭ��" << std::endl;
		Inventory* inven = static_cast<Inventory*>(mParentUI);

		// �κ��丮���� ������ �������� ���� ��
		if (inven->GetPickingItem() != nullptr)
		{
			// ���õ� �κ��丮 ���Կ� �������� ���ٸ�
			// �������� ������ ���Կ� ��ġ��Ų��.
			if (mItem == nullptr)
			{				
				mItem = inven->GetPickingItem();
				mItem->SetPosition(mPos.first, mPos.second);
				// ���� ������ ����ش�.

				int pickingSlotNum = inven->GetPickingSlot();
				inven->FindSlot(pickingSlotNum)->AddItem(nullptr);
			}
			else
			{				
				// ���Կ� �ִ� �����۰� ������ �������� ���� ��ġ�� �ٲ۴�.
				InventoryItem* oldItem = mItem;
				mItem = inven->GetPickingItem();
				mItem->SetPosition(mPos.first, mPos.second);

				int pickingSlotNum = inven->GetPickingSlot();
				inven->FindSlot(pickingSlotNum)->AddItem(oldItem);
			}

			mItem->SetItemDrag(false);
			inven->ResetPicking();			
		}

		// �κ��丮���� ������ �������� ���� ��
		else
		{
			// ���� ���Կ� �������� �ִٸ�
			if (mItem != nullptr)
			{
				std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
				mItem->Click(mousePos.first, mousePos.second);
				mItem->SetItemDrag(true);

				inven->AddPickingItem(mSlotNum, mItem);
				//mItem = nullptr;
			}
		}

		mMouseLButtonClick = false;
	}
}

void InventorySlot::MouseLButtonDoubleClick()
{
	if (mItem == nullptr)
	{
		return;
	}

	// �ƹ�Ÿ�� ������ ����
	Player* player = static_cast<InGameScene*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE))->GetPlayer();
	player->SetAvatarId(mItem->GetItemType(), mItem->GetTexId());

	// ������ �ƹ�Ÿ �ٲ�ٴ� ��Ŷ ����
#ifdef SERVER_CONNECT
	GET_INSTANCE(Network)->SendChangeAvatarPacket(mItem->GetItemType(), mItem->GetTexId());
#endif

	// ���â�� ������ ����
	EquipUI* ui = static_cast<EquipUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("EquipUI"));
	InventoryItem* oldItem = ui->AddEquipItem(mItem);
	AddItem(oldItem);
}

void InventorySlot::SetPosition(int x, int y)
{
	UI::SetPosition(x, y);
	
	if (mItem != nullptr)
	{
		mItem->SetPosition(mPos.first, mPos.second);
	}
}

void InventorySlot::AddItem(int texId)
{
	TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureDataIcon(texId);

	// ������ �߰�
	mItem = new InventoryItem;
	mItem->SetTexId(data.texId);
	mItem->SetItemName(data.name);
	mItem->SetItemType(data.equipSlotType);

	// �ؽ��� �߰�
	mItem->SetTexture(data.name);
	mItem->Visible();
	mItem->SetPosition(mPos.first, mPos.second);
}

void InventorySlot::AddItem(const std::string& name)
{
	// ������ �߰�
	mItem = new InventoryItem;
	mItem->SetItemName(name);
	// �ؽ��� �߰�
	mItem->SetTexture(name);
	mItem->Visible();
	mItem->SetPosition(mPos.first, mPos.second);
}

void InventorySlot::AddItem(InventoryItem* oldItem)
{
	if (oldItem == nullptr)
	{
		mItem = nullptr;
	}
	else
	{
		mItem = oldItem;
		mItem->SetPosition(mPos.first, mPos.second);
	}
}

Inventory::Inventory()
	: UI()
{
	mSlotGap = 0;
	mSlotWidth = 0;
	mSlotHeight = 0;

	mOpen = false;
	mMeso = 100000000;

	mPickingItem = nullptr;
	mPickingSlot = -1;
}

Inventory::~Inventory()
{
}

bool Inventory::Initialize(int x, int y)
{
	UI::Initialize(x, y);
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("InvenUIBackground0");
		SetTexture(data.name);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("InvenUIBackground1");
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
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("InvenUIBackground2");
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
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("InvenUITab00");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("Tab", ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("InvenUITab11");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("Tab", ui);
	}
	for(int i = 0; i < 10; ++i)
	{
		std::string texName = "InvenUIButton" + std::to_string(i);
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(texName);
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("Button", ui);
	}
	{
		int originX = 165;
		int originY = 0;

		ButtonUI* button = new ButtonUI;
		button->Initialize(originX, originY);
		button->SetTexture("XButton");
		button->Visible();
		button->SetLButtonClickCallback(CloseInventory);

		AddChildUI("Button", button);
	}

	{
		mSlotGap = 12;
		mSlotWidth = 30;
		mSlotHeight = 30;

		// ������ ����ǥ
		int originX = 14;
		int originY = 55;
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
				if (slot->SetTexture("InventorySlot") == false)
				{
					return false;
				}

				slot->Visible();
				// ���� ��ȣ
				slot->SetSlotNum(num++);
				// �ڽ� �߰�
				AddChildUI("Slot", slot);

				tempX += (mSlotWidth + mSlotGap);
			}
			tempX = originX;
			tempY += (mSlotHeight + mSlotGap);
		}
	}

	if (MAX_INVENTORY_WIDTH_SLOT_SIZE > VIEW_SLOT_HEIGHT)
	{
		int originX = 175;
		int originY = 55;

		Scroll* scroll = new Scroll;
		scroll->Initialize(originX, originY);

		int alphaValue = ((originY + (VIEW_SLOT_HEIGHT - 1) * (mSlotHeight + 1 + mSlotGap)) - originY) / MAX_INVENTORY_WIDTH_SLOT_SIZE;
		scroll->SetAlphaValue(alphaValue);
		scroll->SetTexture("ScrollBackground");
		scroll->Visible();
		AddChildUI("Scroll", scroll);
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

	// ���̴� ���Ը� �������ϵ���
	for (auto& slot : mChildUIs["Slot"])
	{
		int left = slot->GetPosition().first;
		int top = slot->GetPosition().second;
		int right = left + slot->GetTexture()->GetSize().first;
		int bottom = top + slot->GetTexture()->GetSize().second;
		if (CheckContain(left, top, right, bottom) == false)
		{
			slot->NotVisible();
		}
		else
		{
			slot->Visible();
		}
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

void Inventory::Render()
{
	// ���̴� �͸� ����
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

	{
		// �޼�
		GET_INSTANCE(GraphicEngine)->RenderText(std::to_wstring(mMeso), mPos.first - 152, mPos.second + 305, "������");
		// ����
		GET_INSTANCE(GraphicEngine)->RenderText(L"10,000", mPos.first - 116, mPos.second + 323, "������");
	}
}

void Inventory::MouseOver()
{	
}

void Inventory::MouseLButtonDown()
{
	UI::MouseLButtonDown();
}

void Inventory::MouseLButtonUp()
{
	UI::MouseLButtonUp();
}

void Inventory::MouseLButtonClick()
{
	UI::MouseLButtonClick();
}

void Inventory::ProcessMouseWheelEvent(unsigned long long wParam)
{
	if (mChildUIs.count("Scroll") == false)
	{
		return;
	}

	bool upFlag = false, downFlag = false;
	int min = 0;
	int max = MAX_INVENTORY_WIDTH_SLOT_SIZE - VIEW_SLOT_HEIGHT;
	static int scrollBar = max;

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	if (Collision(mousePos.first, mousePos.second) == false)
	{
		return;
	}

	std::cout << "�κ��丮 �ȿ��� ��ũ�� �̺�Ʈ" << std::endl;
	
	if ((SHORT)HIWORD(wParam) > 0)
	{
		if (scrollBar < max)
		{
			upFlag = true;
		}
	}
	else
	{
		if (scrollBar > min)
		{
			downFlag = true;
		}
	}

	if (upFlag == false && downFlag == false)
	{
		return;
	}

	for (auto& child : mChildUIs["Slot"])
	{
		InventorySlot* slot = static_cast<InventorySlot*>(child);
		std::pair<int, int> originPos = slot->GetOriginPosition();
		if (upFlag == true)
		{
			originPos.second += (slot->GetTexture()->GetSize().second + mSlotGap);
		}
		else
		{
			originPos.second -= (slot->GetTexture()->GetSize().second + mSlotGap);
		}		
		slot->SetOriginPosition(originPos.first, originPos.second);
		std::pair<int, int> pos = slot->GetPosition();
		slot->SetPosition(pos.first, pos.second);
		if (slot->GetItem() != nullptr)
		{
			slot->GetItem()->SetPosition(slot->GetPosition().first, slot->GetPosition().second);
		}
	}

	// ��ũ���� �ִ°�쿡
	if (mChildUIs.count("Scroll") == true)
	{
		// ��ũ�� ��;
		ScrollBar* scrollBars = static_cast<ScrollBar*>(mChildUIs["Scroll"].front()->FindChildUIs("ScrollBar").front());
		std::pair<int, int> scrollBarOriginPos = scrollBars->GetOriginPosition();
		if (upFlag == true)
		{
			++scrollBar;
			scrollBarOriginPos.second -= scrollBars->GetAlphaValue();

			if (scrollBar > max)
			{
				scrollBar = max;
			}
		}
		else if (downFlag == true)
		{
			--scrollBar;
			scrollBarOriginPos.second += scrollBars->GetAlphaValue();

			if (scrollBar < min)
			{
				scrollBar = min;
			}
		}

		scrollBars->SetOriginPosition(scrollBarOriginPos.first, scrollBarOriginPos.second);
		std::pair<int, int> scrollBarPos = scrollBars->GetPosition();
		scrollBars->SetPosition(scrollBarPos.first, scrollBarPos.second);
	}
	else
	{
		if (upFlag == true)
		{
			++scrollBar;
			if (scrollBar > max)
			{
				scrollBar = max;
			}
		}
		else if (downFlag == true)
		{
			--scrollBar;
			if (scrollBar < min)
			{
				scrollBar = min;
			}
		}
	}
}

void Inventory::ProcessMouseDoubleClickEvent()
{
	std::vector<UI*>& slots = FindChildUIs("Slot");
	for (int i = 0; i < slots.size(); ++i)
	{		
		// ����Ŭ�� �̺�Ʈ�� �߻��� ����
		if (slots[i]->GetMouseLButtonDown() == true)
		{
			InventorySlot* slot = static_cast<InventorySlot*>(slots[i]);
			// ����Ŭ�� �̺�Ʈ�� �߻��ߴٰ� ����
			slot->SetMouseLButtonDoubleClick(true);
			break;
		}
	}	
}

bool Inventory::CheckContain(int left, int top, int right, int bottom)
{
	//D2D1_RECT_F rect;
	//rect.left = mPos.first;
	//rect.top = mPos.second + 30;
	//rect.right = rect.left + mTexture->GetSize().first;
	//rect.bottom = rect.top + mTexture->GetSize().second - 60;

	D2D1_RECT_F rect;
	rect.left = mPos.first + 4;
	rect.top = mPos.second + 45;
	rect.right = rect.left + +175;
	rect.bottom = rect.top + 260;

	if (left < rect.left || right > rect.right || top < rect.top || bottom > rect.bottom)
	{
		return false;
	}

	return true;
}

void Inventory::AddItem(int texId)
{
	for (auto& inventorySlot : mChildUIs["Slot"])
	{
		InventorySlot* slot = static_cast<InventorySlot*>(inventorySlot);
		// ����ִ� ���Կ� �߰�
		if (slot->GetItem() == nullptr)
		{
			slot->AddItem(texId);
			break;
		}
	}
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

void Inventory::AddItem(InventoryItem* item)
{
	for (auto& inventorySlot : mChildUIs["Slot"])
	{
		InventorySlot* slot = static_cast<InventorySlot*>(inventorySlot);
		// ����ִ� ���Կ� �߰�
		if (slot->GetItem() == nullptr)
		{
			slot->AddItem(item);
			break;
		}
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

InventorySlot* Inventory::FindSlot()
{
	std::vector<UI*>& v = FindChildUIs("Slot");
	for (int i = 0; i < v.size(); ++i)
	{
		InventorySlot* slot = static_cast<InventorySlot*>(v[i]);
		if (slot->GetItem() == nullptr)
		{
			continue;
		}

		if (slot->GetItem()->GetItemDrag() == true)
		{
			std::cout << i << "�� ���� ������" << std::endl;
			return slot;
		}
	}

	return nullptr;
}

InventorySlot* Inventory::FindSlot(int slotNum)
{
	std::vector<UI*>& v = FindChildUIs("Slot");

	return static_cast<InventorySlot*>(v[slotNum]);
}

void Inventory::DeductMeso(long long meso)
{
	mMeso -= meso;
	if (mMeso < 0)
	{
		mMeso = 0;
	}
}

InventoryItem::InventoryItem()
	: UI()
{
	mItemName.clear();
	mTexId = -1;
	mItemType = 0;

	mItemDrag = false;
}

InventoryItem::~InventoryItem()
{
	mItemName.clear();
}

bool InventoryItem::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	return true;
}

void InventoryItem::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	if (mItemDrag == true)
	{
		std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
		Move(mousePos.first, mousePos.second);
	}
}

void InventoryItem::Render()
{
	// ���̴� �͸� ����
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

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}