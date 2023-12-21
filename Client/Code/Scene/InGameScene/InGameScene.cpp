#include "InGameScene.h"
#include "../../Common/Defines.h"
#include "../../Input/Input.h"
#include "../../GameObject/Map/Map.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../GameObject/Character/Character.h"

#include "../../Manager/UIManager/UIManager.h"
#include "../../GameObject/UI/ChattingBox/ChattingBox.h"
#include "../../GameObject/UI/InputUI/ChattingInputUI/ChattingInputUI.h"
#include "../../GameObject/UI/Inventory/Inventory.h"
#include "../../GameObject/UI/EquipUI/EquipUI.h"

#include "../../../../Server/Code/Common/Protocol.h"

#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"
#include <random>

InGameScene::InGameScene()
	: Scene()
{
	mTileMinPos = std::make_pair(0, 0);
	mTileMaxPos = std::make_pair(0, 0);
	mTiles.clear();
	mPlayer = nullptr;
	mIsReady = false;
}

InGameScene::~InGameScene()
{
	for (auto& ui : mUIs)
	{
		delete ui;
	}
	mUIs.clear();
	mUIsMap.clear();

	for (int i = 0; i < mTiles.size(); ++i)
	{
		for (int j = 0; j < mTiles[i].size(); ++j)
		{
			delete mTiles[i][j];
		}
	}

	mOtherPlayers.clear();
	mMonsters.clear();
	delete mPlayer;
}

bool InGameScene::Initialize()
{
	for (int i = 0; i < WIDTH; ++i)
	{
		std::vector<Map*> v;
		for (int j = 0; j < HEIGHT; ++j)
		{
			Map* tile = new Map;
			if (tile->Initialize(i, j) == false)
			{
				return false;
			}
			if (tile->SetTexture("Tile") == false)
			{
				return false;
			}
			v.emplace_back(tile);
		}
		mTiles.emplace_back(v);
		v.clear();
	}

	mPlayer = new Player;
	if (mPlayer->Initialize(0, 0) == false)
	{
		return false;
	}
	mPlayer->Visible();
	mPlayer->SetId(-1);

	mTileMinPos = std::make_pair(mPlayer->GetPosition().first - VIEW_DISTANCE, mPlayer->GetPosition().second - VIEW_DISTANCE);
	mTileMaxPos = std::make_pair(mPlayer->GetPosition().first + VIEW_DISTANCE, mPlayer->GetPosition().second + VIEW_DISTANCE);

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(0, WIDTH - 1);

	// 나를 제외한 나머지 유저 수
	for (int i = 0; i < MAX_CHANNEL_USER; ++i)
	{
		std::shared_ptr<AnimationCharacter> otherPlayer = std::make_shared<AnimationCharacter>();
		if (otherPlayer->Initialize(0, 0) == false)
		{
			return false;
		}
		otherPlayer->SetId(i);
#ifdef SERVER_CONNECT
		otherPlayer->NotVisible();
#else
		otherPlayer->Visible();
		otherPlayer->SetPosition(uid(dre), uid(dre));
#endif
		mOtherPlayers.emplace(i, otherPlayer);
	}

	// 몬스터
	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		std::shared_ptr<Monster> monster = std::make_shared<Monster>();
		if (monster->Initialize(0, 0) == false)
		{
			return false;
		}
		monster->SetId(i);
#ifdef SERVER_CONNECT
		monster->NotVisible();
#else
		monster->Visible();
		monster->SetPosition(uid(dre), uid(dre));
#endif
		mMonsters.emplace(i, monster);
	}

	ChattingBox* mChattingBox = new ChattingBox;
	if (mChattingBox->Initialize(0, 0) == false)
	{
		return false;
	}
	AddSceneUI("ChattingBox", mChattingBox);

	EquipUI* equipUI = new EquipUI;
	if (equipUI->Initialize(0, 0) == false)
	{
		return false;
	}
	AddSceneUI("EquipUI", equipUI);

	Inventory* inventory = new Inventory;
	if (inventory->Initialize(0, 0) == false)
	{
		return false;
	}
	AddSceneUI("Inventory", inventory);

	mIsReady = true;

	return true;
}

void InGameScene::Update()
{	
	processKeyboardMessage();

	if (mIsReady == false)
	{
		return;
	}

	// update
	mPlayer->Update();

	// 플레이어가 볼수 있는 타일만 업데이트	
	mTileMinPos = std::make_pair(mPlayer->GetPosition().first - VIEW_DISTANCE, mPlayer->GetPosition().second - VIEW_DISTANCE);
	mTileMaxPos = std::make_pair(mPlayer->GetPosition().first + VIEW_DISTANCE, mPlayer->GetPosition().second + VIEW_DISTANCE);
	for (int i = mTileMinPos.first; i <= mTileMaxPos.first; ++i)
	{
		for (int j = mTileMinPos.second; j <= mTileMaxPos.second; ++j)
		{
			if (checkRange(i, j) == false)
			{
				continue;
			}
			mTiles[i][j]->Visible();
			mTiles[i][j]->Update();
		}
	}

	for (auto& monster : mMonsters)
	{
		monster.second->Update();
	}

	for (auto& otherPlayer : mOtherPlayers)
	{
		otherPlayer.second->Update();
	}

	GET_INSTANCE(UIManager)->Update();
}

void InGameScene::Render()
{
	if (mIsReady == false)
	{
		return;
	}

	for (int i = mTileMinPos.first; i <= mTileMaxPos.first; ++i)
	{
		for (int j = mTileMinPos.second; j <= mTileMaxPos.second; ++j)
		{
			if (checkRange(i, j) == false)
			{
				continue;
			}
			mTiles[i][j]->Render();
		}
	}

	for (auto& monster : mMonsters)
	{
		monster.second->Render();
	}

	for (auto& player : mOtherPlayers)
	{
		player.second->Render();
	}

	mPlayer->Render();
	{
		D2D1_RECT_F rect;
		rect.left = 45;
		rect.top = 660;
		rect.right = rect.left + 245;
		rect.bottom = rect.top + 20;
		std::wstring text = L"Lv 200";
		GET_INSTANCE(GraphicEngine)->RenderText(text, 0, rect.top, "메이플", "빨간색");
	}
	{
		D2D1_RECT_F rect;
		rect.left = 45;
		rect.top = 685;
		rect.right = rect.left + 245;
		rect.bottom = rect.top + 20;
		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(rect, "빨간색");
		GET_INSTANCE(GraphicEngine)->RenderText(L"HP", 0, rect.top, "메이플", "빨간색");
		GET_INSTANCE(GraphicEngine)->RenderText(L"50", 145, rect.top - 2, "메이플", "흰색");
	}
	{
		D2D1_RECT_F rect;
		rect.left = 45;
		rect.top = 710;
		rect.right = rect.left + 245;
		rect.bottom = rect.top + 20;
		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(rect, "파란색");
		GET_INSTANCE(GraphicEngine)->RenderText(L"MP", 0, rect.top, "메이플", "파란색");
		GET_INSTANCE(GraphicEngine)->RenderText(L"50", 145, rect.top - 2, "메이플", "흰색");
	}

	{
		D2D1_RECT_F rect;
		rect.left = 55;
		rect.top = 735;
		rect.right = rect.left +745;
		rect.bottom = rect.top + 20;
		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(rect, "주황색");
		GET_INSTANCE(GraphicEngine)->RenderText(L"EXP", 0, rect.top, "메이플", "주황색");
		GET_INSTANCE(GraphicEngine)->RenderText(L"100%", 380, rect.top - 2, "메이플", "흰색");
	}

	GET_INSTANCE(UIManager)->Render();

	int x = 400;
	int y = 200;
	//SetAvatarPose0(x, y);
	//SetAvatarPose1(x + 50, y);
	//SetAvatarPose2(x + 100, y);
	//SetAvatarPose3(x + 150, y);
	//SetMonsterPose0(x, y);
	//SetMonsterPose1(x, y);
	//SetInventory(x, y);
}

void InGameScene::processKeyboardMessage()
{
	ChattingBox* chattingBoxUI = static_cast<ChattingBox*>(FindUI("ChattingBox"));
	if (chattingBoxUI == nullptr)
	{
		return;
	}

	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::ENTER_KEY) == true)
	{
		chattingBoxUI->OpenChattingBox();
	}
	else
	{
		// 채팅상태가 아닐 때
		if (static_cast<ChattingInputUI*>(chattingBoxUI->FindChildUIs("ChattingInputUI").front())->IsVisible() == false)
		{			
			if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::I_KEY) == true)
			{
				Inventory* inventory = static_cast<Inventory*>(FindUI("Inventory"));
				inventory->OpenInventory();
				GET_INSTANCE(UIManager)->SetFocusUI(inventory);
			}
			else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::Z_KEY) == true)
			{
				mPlayer->AddItem();
			}
			else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::E_KEY) == true)
			{
				EquipUI* equipUI = static_cast<EquipUI*>(FindUI("EquipUI"));
				equipUI->OpenEquipUI();
				GET_INSTANCE(UIManager)->SetFocusUI(equipUI);
			}
			else
			{
				mPlayer->ProcessKeyboardMessage();
			}
		}
	}
}

void InGameScene::InitializeObject(int myId)
{
	mPlayer->SetId(myId);
	mOtherPlayers.erase(myId);
}

void InGameScene::AddObject(int id, int x, int y)
{
	int myId = mPlayer->GetId();
	if (myId == id)
	{
		//mPlayer->SetWeaponAvatar("Sword");
		GET_INSTANCE(Camera)->SetPosition(x, y);
		mPlayer->SetPosition(x, y);
	}
	else if (id < MAX_USER)
	{
		//mOtherPlayers[id]->SetAvatar()
		mOtherPlayers[id]->SetPosition(x, y);
		mOtherPlayers[id]->Visible();
	}
	else
	{
		mMonsters[id]->SetPosition(x, y);
		mMonsters[id]->Visible();
	}
}

void InGameScene::UpdateObjectPosition(int id, int x, int y)
{
	int myId = mPlayer->GetId();
	if (myId == id)
	{
		GET_INSTANCE(Camera)->SetPosition(x, y);
		mPlayer->SetPosition(x, y);
	}
	else if (id < MAX_USER)
	{
		mOtherPlayers[id]->SetPosition(x, y);
		mOtherPlayers[id]->Visible();
	}
	else
	{
		mMonsters[id]->SetPosition(x, y);
		mMonsters[id]->Visible();
	}
}

void InGameScene::RemoveObject(int id)
{
	int myId = mPlayer->GetId();
	if (id < MAX_USER)
	{
		mOtherPlayers[id]->NotVisible();
	}
	else
	{
		mMonsters[id]->NotVisible();
	}
}

void InGameScene::UpdateObjectAvatar(int id, int texId)
{
	// texId를 이용하여 아이템 이름 구분
	mOtherPlayers[id]->SetAvatar(texId);
}

Player* InGameScene::GetPlayer()
{
	return mPlayer;
}

AnimationCharacter* InGameScene::GetOtherPlayer(int id)
{
	return mOtherPlayers[id].get();
}

Monster* InGameScene::GetMonster(int id)
{
	return mMonsters[id].get();
}

void InGameScene::SetAvatarPose0(int x, int y)
{
	// head
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("FrontHead0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	// body
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("JumpBody0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// equipBody
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("EquipBodyJumpDreamOfDoll0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// arm
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("JumpArm0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// equipArm
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("EquipArmJumpDreamOfDoll0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// leftHand
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("JumpLeftHand0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

}

void InGameScene::SetAvatarPose1(int x, int y)
{
	// head
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("FrontHead0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// body
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("IdleBody1");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// equipBody
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("EquipBodyIdleDreamOfDoll1");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// arm
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("IdleArm1");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// equipArm
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("EquipArmIdleDreamOfDoll1");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
}

void InGameScene::SetAvatarPose2(int x, int y)
{
	// head
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("FrontHead0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	// body
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("IdleBody2");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// equipBody
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("EquipBodyIdleDreamOfDoll2");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// arm
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("IdleArm2");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// equipArm
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("EquipArmIdleDreamOfDoll2");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
}

void InGameScene::SetAvatarPose3(int x, int y)
{
	// head
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("FrontHead0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	// body
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("WalkBody3");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// equipBody
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("EquipBodyWalkDreamOfDoll3");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// arm
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("WalkArm3");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	// equipArm
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("EquipArmWalkDreamOfDoll3");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
}

void InGameScene::SetMonsterPose0(int x, int y)
{
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("IdleOrangeMushroom0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
}

void InGameScene::SetMonsterPose1(int x, int y)
{
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("IdleOrangeMushroom1");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
}

void InGameScene::SetInventory(int x, int y)
{
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUIBackground0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUIBackground1");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUIBackground2");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUITab00");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUITab11");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUIButton0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUIButton1");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUIButton2");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUIButton3");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUIButton4");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUIButton5");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUIButton6");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUIButton7");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUIButton8");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("InvenUIButton9");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
}

bool InGameScene::checkRange(int x, int y)
{
	if (x < 0 || x > WIDTH - 1 || y < 0 || y > HEIGHT - 1)
	{
		return false;
	}

	return true;
}
