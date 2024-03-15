#include "TradeUI.h"
#include "../../../Network/Network.h"

#include "../ButtonUI/ButtonUI.h"
#include "../DialogUI/TradeMesoDialog/TradeMesoDialog.h"

#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../Resource/ResourceManager.h"
#include "../../../Resource/Texture/Texture.h"

#include "../../../Manager/SceneMangaer/SceneManager.h"
#include "../../../Scene/InGameScene/InGameScene.h"
#include "../Inventory/Inventory.h"
#include "../../../Manager/UIManager/UIManager.h"

#include "../../Character/Character.h"

void TradeCancelClick(const std::string& name)
{
	TradeUI* trade = static_cast<TradeUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("TradeUI"));

#ifdef SERVER_CONNECT
	// 취소 버튼 클릭 전송
	std::cout << "TRADE CANCEL SEND - userId : " << trade->GetTradeUserId() << std::endl;
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

	// 내가 올린 아이템 아이디를 저장할 배열
	int items[MAX_TRADE_SLOT];
	memset(items, -1, sizeof(items));

	std::vector<UI*>& v = ui->FindChildUIs("Slot");
	for (int i = 0; i < MAX_TRADE_SLOT; ++i)
	{
		TradeSlotUI* slot = static_cast<TradeSlotUI*>(v[i]);
		if (slot->GetItem() == nullptr)
		{
			continue;
		}
		items[i] = slot->GetItem()->GetTexId();
	}

#ifdef SERVER_CONNECT
	GET_INSTANCE(Network)->SendTradePacket(ui->GetTradeUserId(), items, ui->GetMyMeso());
#endif // SERVER_CONNECT
}

void AddMesoClick(const std::string& name)
{
	TradeUI* trade = static_cast<TradeUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("TradeUI"));
	trade->GetMesoDialogUI()->OpenDialogUI();
	GET_INSTANCE(UIManager)->SetFocusUI(trade->GetMesoDialogUI());
}

TradeUI::TradeUI()
	: UI()
{
	mOpen = false;
	mTradeUserId = -1;
	mMyMeso = 0;
	mTradeUserMeso = 0;
	mMesoDialogUI = nullptr;
	mMyAvatar = nullptr;
	mTradeUserAvatar = nullptr;
}

TradeUI::~TradeUI()
{
	if (mMesoDialogUI != nullptr)
	{
		mMesoDialogUI = nullptr;
	}

	if (mMyAvatar != nullptr)
	{
		delete mMyAvatar;
		mMyAvatar = nullptr;
	}

	if (mTradeUserAvatar != nullptr)
	{
		delete mTradeUserAvatar;
		mTradeUserAvatar = nullptr;
	}
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
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("TradeUIButton2");
		ButtonUI* ui = new ButtonUI;
		if(ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}	
		ui->SetTexture(data.name);
		ui->Visible();
		ui->SetLButtonClickCallback(AddMesoClick);
		AddChildUI("Button", ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("TradeUIButton3");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
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

	mMesoDialogUI = new TradeMesoDialog;
	if (mMesoDialogUI->Initialize(200, 0) == false)
	{
		return false;
	}
	GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->AddSceneUI("MesoDialogUI", mMesoDialogUI);

	//Visible();
	SetPosition(140, 150);

	return true;
}

void TradeUI::Update()
{
	UI::Update();

	if (mMyAvatar != nullptr)
	{
		mMyAvatar->Update();
	}
	if (mTradeUserAvatar != nullptr)
	{
		mTradeUserAvatar->Update();
	}
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

	{
		std::wstring myMeso = std::to_wstring(mMyMeso);
		std::wstring myTradeUserMeso = std::to_wstring(mTradeUserMeso);

		GET_INSTANCE(GraphicEngine)->RenderText(myMeso, mPos.first - 41, mPos.second + 260, "검은색");
		GET_INSTANCE(GraphicEngine)->RenderText(myTradeUserMeso, mPos.first - 180, mPos.second + 260, "검은색");
	}

	InGameScene* scene = static_cast<InGameScene*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE));
	if (mMyAvatar != nullptr)
	{
		mMyAvatar->AvatarRender(340, 200);
		GET_INSTANCE(GraphicEngine)->RenderText(std::to_wstring(scene->GetPlayer()->GetId()), mPos.first + 215, mPos.second + 121);
	}
	if(mTradeUserAvatar != nullptr)
	{
		mTradeUserAvatar->AvatarRender(210, 200);
		//GET_INSTANCE(GraphicEngine)->RenderText(std::to_wstring(scene->GetOtherPlayer(mTradeUserId)->GetId()), mPos.first + 52, mPos.second + 121);
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
		SetAvatar();
	}
	// 열려있다면
	else
	{
		mOpen = false;
		NotVisible();
		ResetAvatar();
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
	// 상대방 슬롯은 나의 슬롯번호 + 9
	int index = slotNum + MAX_TRADE_SLOT;
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

	mMyMeso = 0;
	mTradeUserMeso = 0;

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

	// 내 아이템을 인벤토리에 원위치 시켜야함
	Inventory* inventory = static_cast<Inventory*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("Inventory"));
	for (int i = 0; i < MAX_TRADE_SLOT; ++i)
	{
		TradeSlotUI* slot = static_cast<TradeSlotUI*>(v[i]);
		if (slot->GetItem() == nullptr)
		{
			continue;
		}
		inventory->AddItem(slot->GetItem());
		// 슬롯의 아이템 null로 초기화
		slot->SetItem();
	}

	// 상대방 교환 슬롯
	for (int i = MAX_TRADE_SLOT; i < v.size(); ++i)
	{
		static_cast<TradeSlotUI*>(v[i])->ResetSlot();
	}

	// 나의 메소도 원위치 시켜야함
	inventory->AddMeso(mMyMeso);
	mMyMeso = 0;
	mTradeUserMeso = 0;

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

void TradeUI::SetAvatar()
{
	// 나의 아바타 설정
	{
		mMyAvatar = new AnimationCharacter;
		if (mMyAvatar->Initialize(0, 0) == false)
		{
			return;
		}

		InGameScene* scene = static_cast<InGameScene*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE));
		std::vector<int>& avatarIds = scene->GetPlayer()->GetAvatarIds();
		for (int i = 0; i < avatarIds.size(); ++i)
		{
			mMyAvatar->SetAvatarId(i, avatarIds[i]);
		}

		mMyAvatar->SetAnimationMotion(ANIMATION_MOTION_TYPE::IDLE);
		mMyAvatar->Visible();
		//mMyAvatar->SetPosition(1, -1);
	}

	// 상대 아바타 설정
	{
		mTradeUserAvatar = new AnimationCharacter;
		if (mTradeUserAvatar->Initialize(0, 0) == false)
		{
			return;
		}

		InGameScene* scene = static_cast<InGameScene*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE));
		//std::vector<int>& avatarIds = scene->GetOtherPlayer(mTradeUserId)->GetAvatarIds();
		std::vector<int> avatarIds;
		for (int i = 0; i < avatarIds.size(); ++i)
		{
			mTradeUserAvatar->SetAvatarId(i, avatarIds[i]);
		}

		mTradeUserAvatar->SetAnimationMotion(ANIMATION_MOTION_TYPE::IDLE);
		mTradeUserAvatar->Visible();
		mTradeUserAvatar->SetDirection(-1);
		//mTradeUserAvatar->SetPosition(-2, -1);
	}
}

void TradeUI::ResetAvatar()
{
	if (mMyAvatar != nullptr)
	{
		delete mMyAvatar;
		mMyAvatar = nullptr;
	}

	if (mTradeUserAvatar != nullptr)
	{
		delete mTradeUserAvatar;
		mTradeUserAvatar = nullptr;
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

#ifdef SERVER_CONNECT
	int userId = static_cast<TradeUI*>(mParentUI)->GetTradeUserId();
	// 아이템을 올렸다고 패킷 전송
	GET_INSTANCE(Network)->SendAddTradeItem(userId, mItem->GetTexId(), mSlotNum);
#endif // SERVER_CONNECT
}

void TradeSlotUI::AddItem(int texId)
{
	TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureDataIcon(texId);

	// 아이템 추가
	mItem = new InventoryItem;
	if (mItem->Initialize(0, 0) == false)
	{
		return;
	}
	mItem->SetTexId(data.texId);
	mItem->SetItemName(data.name);
	mItem->SetItemType(data.equipSlotType);

	// 텍스쳐 추가
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