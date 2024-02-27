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
#include "../../GameObject/UI/SkillUI/SkillUI.h"
#include "../../GameObject/Skill/Skill.h"

#include "../../GameObject/UI/TradeUI/TradeUI.h"
#include "../../GameObject/UI/GameMenuUI/GameMenuUI.h"
#include "../../GameObject/UI/ChannelUI/ChannelUI.h"

#include "../../../../Server/Code/Common/Protocol.h"
#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"
#include "../../Common/Utility.h"

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

	mVisibleOtherPlayers.clear();
	mVisibleMonsters.clear();
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

	// 나를 제외한 나머지 유저 수
	for (int i = 0; i < MAX_CHANNEL * MAX_CHANNEL_USER; ++i)
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
		//otherPlayer->Visible();
		otherPlayer->SetPosition(GetRandomNumber(0, WIDTH -1), GetRandomNumber(0, HEIGHT - 1));
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
		//monster->Visible();
		monster->SetPosition(GetRandomNumber(0, WIDTH - 1), GetRandomNumber(0, HEIGHT - 1));
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

	SkillUI* skillUI = new SkillUI;
	if (skillUI->Initialize(0, 0) == false)
	{
		return false;
	}
	AddSceneUI("SkillUI", skillUI);

	TradeUI* trade = new TradeUI;
	trade->Initialize(0, 0);
	AddSceneUI("TradeUI", trade);

	InGameMenuUI* gameMenuUI = new InGameMenuUI;
	if (gameMenuUI->Initialize(0, 0) == false)
	{
		return false;
	}
	AddSceneUI("InGameMenuUI", gameMenuUI);

	ChannelUI* channelUI = new ChannelUI;
	if (channelUI->Initialize(0, 0) == false)
	{
		return false;
	}
	AddSceneUI("ChannelUI", channelUI);

	//{
	//	Map* tile = new Map;
	//	if (tile->Initialize(0, 0) == false)
	//	{
	//		return false;
	//	}
	//	if (tile->SetTexture("Tile") == false)
	//	{
	//		return false;
	//	}
	//	mNewTiles.emplace_back(tile);
	//}

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

	for (auto& skill : mSkills)
	{
		skill->Update();
	}

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

#ifdef SERVER_CONNECT
	for (auto& monster : mVisibleMonsters)
	{
		monster.second->Update();
	}

	for (auto& otherPlayer : mVisibleOtherPlayers)
	{
		otherPlayer.second->Update();
	}

#else
	for (auto& monster : mMonsters)
	{
		if (mPlayer->CheckDistance(monster.second->GetPosition().first, monster.second->GetPosition().second) == true)
		{
			monster.second->Visible();
			monster.second->Update();
		}
		else
		{
			monster.second->NotVisible();
		}
	}

	for (auto& otherPlayer : mOtherPlayers)
	{
		if (mPlayer->CheckDistance(otherPlayer.second->GetPosition().first, otherPlayer.second->GetPosition().second) == true)
		{
			otherPlayer.second->Visible();
			otherPlayer.second->Update();
		}
		else
		{
			otherPlayer.second->NotVisible();
		}
	}
#endif

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

#ifdef SERVER_CONNECT
	for (auto& monster : mVisibleMonsters)
	{
		monster.second->Render();
	}

	for (auto& player : mVisibleOtherPlayers)
	{
		player.second->Render();
	}
#else
	for (auto& monster : mMonsters)
	{
		monster.second->Render();
	}

	for (auto& player : mOtherPlayers)
	{
		player.second->Render();
	}
#endif // SERVER_CONNECT

	for (auto& skill : mSkills)
	{
		skill->Render();
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
	//SetSkillUI(x, y);
	//SetSkillEffect(x, y);

	//{
	//	Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("Player");
	//	std::pair<float, float> pos = std::make_pair(0.0, 0.0);
	//	std::pair<int, int> origin = std::make_pair(10, 0);
	//	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();
	//	char dir = -1;

	//	D2D1_MATRIX_3X2_F mat = D2D1::Matrix3x2F::Identity();
	//	mat._11 *= dir;
	//	mat._31 = origin.first * dir + (pos.first - cameraPos.first) * 65 + 8.0;
	//	mat._32 = origin.second + (pos.second - cameraPos.second) * 65 + 8.0;
	//	if (dir == -1)
	//	{
	//		mat._31 += tex->GetSize().first - origin.first * dir;
	//	}

	//	GET_INSTANCE(GraphicEngine)->GetRenderTarget()->SetTransform(mat);

	//	D2D1_RECT_F rect;
	//	rect.left = 0;
	//	rect.top = 0;
	//	rect.right = rect.left + tex->GetSize().first;
	//	rect.bottom = rect.top + tex->GetSize().second;
	//	//GET_INSTANCE(GraphicEngine)->RenderRectangle(rect);
	//	GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);

	//	mat = D2D1::Matrix3x2F::Identity();
	//	GET_INSTANCE(GraphicEngine)->GetRenderTarget()->SetTransform(mat);
	//}

	//{
	//	ID2D1GeometrySink* sink = GET_INSTANCE(GraphicEngine)->GetSink();

	//	sink->SetFillMode(D2D1_FILL_MODE_WINDING);
	//	sink->BeginFigure(D2D1::Point2F(50, 0), D2D1_FIGURE_BEGIN_FILLED);

	//	D2D1_POINT_2F points[] =
	//	{
	//	   D2D1::Point2F(50, 0),
	//	   D2D1::Point2F(100, 50),
	//	   D2D1::Point2F(100, 100),
	//	   D2D1::Point2F(50, 150),
	//	};

	//	sink->AddLines(points, ARRAYSIZE(points));
	//	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	//	sink->Close();

	//	GET_INSTANCE(GraphicEngine)->RenderGeometry();
	//}
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
			if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::ESC_KEY) == true)
			{
				ChannelUI* channelUI = static_cast<ChannelUI*>(FindUI("ChannelUI"));
				if (channelUI->IsVisible() == true)
				{
					ChannelCancelClick("");
					return;
				}

				InGameMenuUI* gameMenuUI = static_cast<InGameMenuUI*>(FindUI("InGameMenuUI"));
				gameMenuUI->OpenGameMenuUI();
				GET_INSTANCE(UIManager)->SetFocusUI(gameMenuUI);
			}
			else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::I_KEY) == true)
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
			else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::K_KEY) == true)
			{
				SkillUI* skillUI = static_cast<SkillUI*>(FindUI("SkillUI"));
				skillUI->OpenSkillUI();
				GET_INSTANCE(UIManager)->SetFocusUI(skillUI);
			}
			else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::CONTROL_KEY) == true)
			{
				Skill* skill = static_cast<SkillUI*>(FindUI("SkillUI"))->FindSkill("레이징 블로우");
				if (skill != nullptr)
				{
					skill->UseSkill();
				}
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

void InGameScene::AddPlayer(int id, int x, int y, int* texIds)
{
	int myId = mPlayer->GetId();
	if (myId == id)
	{
		for (int i = 0; i < MAX_AVATAR_SLOT_SIZE; ++i)
		{
			if (texIds[i] == 0)
			{
				continue;
			}
			static_cast<EquipUI*>(FindUI("EquipUI"))->AddEquipItem(i, texIds[i]);
			mPlayer->SetAvatarId(texIds[i]);
		}

		GET_INSTANCE(Camera)->SetPosition(x, y);
		mPlayer->SetPosition(x, y);
	}
	else if (id < MAX_USER)
	{
		std::shared_ptr<AnimationCharacter> otherPlayer = mOtherPlayers[id];
		otherPlayer->Visible();
		otherPlayer->SetPosition(x, y);
		for (int i = 0; i < MAX_AVATAR_SLOT_SIZE; ++i)
		{
			if (texIds[i] == 0)
			{
				continue;
			}
			otherPlayer->SetAvatarId(texIds[i]);
		}

		mVisibleOtherPlayers.emplace(id, otherPlayer);		
	}
}

void InGameScene::AddObject(int id, int x, int y)
{
	std::shared_ptr<Monster> monster = mMonsters[id];
	monster->Visible();
	monster->SetPosition(x, y);
	mVisibleMonsters.emplace(id, monster);
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
		if (mVisibleOtherPlayers.count(id) == true)
		{
			mVisibleOtherPlayers[id]->SetPosition(x, y);
		}
	}
	else
	{
		if (mVisibleMonsters.count(id) == true)
		{
			mVisibleMonsters[id]->SetPosition(x, y);
		}
	}
}

void InGameScene::RemoveObject(int id)
{
	int myId = mPlayer->GetId();
	if (id < MAX_USER)
	{
		if (mVisibleOtherPlayers.count(id) == true)
		{
			mVisibleOtherPlayers[id]->NotVisible();
			mVisibleOtherPlayers.erase(id);
		}
	}
	else
	{
		if (mVisibleMonsters.count(id) == true)
		{
			mVisibleMonsters[id]->NotVisible();
			mVisibleMonsters.erase(id);
		}
	}
}

void InGameScene::RemoveAllObject()
{
	for (auto& otherPlayer : mVisibleOtherPlayers)
	{
		otherPlayer.second->NotVisible();
	}
	for (auto& monster : mVisibleMonsters)
	{
		monster.second->NotVisible();
	}
	mVisibleOtherPlayers.clear();
	mVisibleMonsters.clear();
}

void InGameScene::UpdateObjectAvatar(int id, int texId)
{
	// texId를 이용하여 아이템 이름 구분
	mOtherPlayers[id]->SetAvatarId(texId);
}

void InGameScene::RequestTrade(int id)
{
	TradeUI* trade = static_cast<TradeUI*>(FindUI("TradeUI"));
	trade->SetTradeUserId(id);
	trade->OpenTradeUI();
}

void InGameScene::AddTradeItem(int texId, int slotNum)
{
	TradeUI* trade = static_cast<TradeUI*>(FindUI("TradeUI"));
	trade->AddItemOfTradeUser(texId, slotNum);
}

void InGameScene::AddTradeMeso(long long meso)
{
	TradeUI* trade = static_cast<TradeUI*>(FindUI("TradeUI"));
	trade->AddTradeUserMeso(meso);
}

void InGameScene::TradeItems(int* items, long long meso)
{
	Inventory* inventory = static_cast<Inventory*>(FindUI("Inventory"));
	for (int i = 0; i < MAX_TRADE_SLOT; ++i)
	{
		if (items[i] == -1)
		{
			continue;
		}
		std::cout << "TradeItems - texId : " << items[i] << std::endl;
		inventory->AddItem(items[i]);
	}
	inventory->AddMeso(meso);

	TradeUI* trade = static_cast<TradeUI*>(FindUI("TradeUI"));
	trade->TradePostProcessing();
}

void InGameScene::TradeCancel()
{
	TradeUI* trade = static_cast<TradeUI*>(FindUI("TradeUI"));
	trade->ProcessTradeCancel();
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

void InGameScene::AddSkill(Skill* skill)
{
	mSkills.emplace_back(skill);
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

void InGameScene::SetSkillUI(int x, int y)
{
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUIBackground0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUIBackground1");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUITab02");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUITab12");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUITab22");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUITab32");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUITab40");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUITab52");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUITab62");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUIBackground2");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUIBackground3");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUIButton0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUIButton1");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUIButton2");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUIButton3");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUIButton4");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUISlot00");
		int originX = tex->GetOrigin().first;
		int originY = tex->GetOrigin().second;
		int gap = 3;
		for (int i = 0; i < 5; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				D2D1_RECT_F rect;
				rect.left = x + originX + (j * (tex->GetSize().first + gap));
				rect.top = y + originY + (i * (tex->GetSize().second + gap));
				rect.right = rect.left + tex->GetSize().first;
				rect.bottom = rect.top + tex->GetSize().second;
				GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
			}
		}
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUISlot1");
		int originX = tex->GetOrigin().first;
		int originY = tex->GetOrigin().second;
		int gap = 3;
		for (int i = 0; i < 2; ++i)
		{
			D2D1_RECT_F rect;
			rect.left = x + originX + (i * (tex->GetSize().first + gap));
			rect.top = y + originY;
			rect.right = rect.left + tex->GetSize().first;
			rect.bottom = rect.top + tex->GetSize().second;
			GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
		}
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("Icon0");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("SkillUIButton50");
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

}

void InGameScene::SetSkillEffect(int x, int y)
{
	//for(int i = 0; i < 14; ++i)
	//{
	//	std::string name = "Effect0" + std::to_string(i);
	//	Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture(name);
	//	D2D1_RECT_F rect;
	//	rect.left = x + tex->GetOrigin().first;
	//	rect.top = y + tex->GetOrigin().second;
	//	rect.right = rect.left + tex->GetSize().first;
	//	rect.bottom = rect.top + tex->GetSize().second;
	//	GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	//}
	{
		std::string name = "Effect1_16";
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture(name);
		D2D1_RECT_F rect;
		rect.left = x + tex->GetOrigin().first;
		rect.top = y + tex->GetOrigin().second;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		std::string name = "Effect1_17";
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture(name);
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
