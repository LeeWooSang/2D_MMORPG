#include "InGameScene.h"
#include "../../Common/Defines.h"
#include "../../Input/Input.h"
#include "../../GameObject/Map/Map.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../GameObject/Character/Character.h"
#include "../../GameObject/UI/ChattingBox/ChattingBox.h"
#include "../../GameObject/UI/InputUI/ChattingInputUI/ChattingInputUI.h"

#include "../../GameObject/UI/UIManager.h"
#include "../../../../Server/Code/Common/Protocol.h"

#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"
#include "../../GameObject/GameObject.h"

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
	mOtherPlayers.clear();
	mMonsters.clear();

	for (int i = 0; i < mTiles.size(); ++i)
	{
		for (int j = 0; j < mTiles[i].size(); ++j)
		{
			delete mTiles[i][j];
		}
	}

	for (auto& npc : mNPCs)
	{
		delete npc;
	}

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

	// ���� ������ ������ ���� ��
	for (int i = 0; i < MAX_CHANNEL_USER; ++i)
	{
		//if (i == myId)
		//{
		//	continue;
		//}

		std::shared_ptr<Character> player = std::make_shared<Character>();
		if (player->Initialize(0, 0) == false)
		{
			return false;
		}
		if (player->SetTexture("OtherPlayer") == false)
		{
			return false;
		}
		player->SetId(i);
		mOtherPlayers.emplace(i, player);
	}

	// ����
	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		std::shared_ptr<Character> monster = std::make_shared<Character>();
		if (monster->Initialize(0, 0) == false)
		{
			return false;
		}
		if (monster->SetTexture("Monster") == false)
		{
			return false;
		}
		monster->SetId(i);
		mMonsters.emplace(i, monster);
	}

	ChattingBox* mChattingBox = new ChattingBox;
	if (mChattingBox->Initialize(0, 0) == false)
	{
		return false;
	}
	GET_INSTANCE(UIManager)->AddUI("ChattingBox", mChattingBox);

	for (int i = 0; i < 10; ++i)
	{
		AnimationCharacter* npc = new AnimationCharacter;
		if (npc->Initialize(0, 0) == false)
		{
			return false;
		}
		npc->SetPosition(i, i);
		mNPCs.emplace_back(npc);
	}

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

	// �÷��̾ ���� �ִ� Ÿ�ϸ� ������Ʈ	
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

	for (auto& npc : mNPCs)
	{
		npc->Update();
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

	for (auto& npc : mNPCs)
	{
		npc->Render();
	}

	mPlayer->Render();
	{
		D2D1_RECT_F rect;
		rect.left = 45;
		rect.top = 660;
		rect.right = rect.left + 245;
		rect.bottom = rect.top + 20;
		std::wstring text = L"Lv 200";
		GET_INSTANCE(GraphicEngine)->RenderText(text, 0, rect.top, "������", "������");
	}
	{
		D2D1_RECT_F rect;
		rect.left = 45;
		rect.top = 685;
		rect.right = rect.left + 245;
		rect.bottom = rect.top + 20;
		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(rect, "������");
		GET_INSTANCE(GraphicEngine)->RenderText(L"HP", 0, rect.top, "������", "������");
		GET_INSTANCE(GraphicEngine)->RenderText(L"50", 145, rect.top - 2, "������", "���");
	}
	{
		D2D1_RECT_F rect;
		rect.left = 45;
		rect.top = 710;
		rect.right = rect.left + 245;
		rect.bottom = rect.top + 20;
		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(rect, "�Ķ���");
		GET_INSTANCE(GraphicEngine)->RenderText(L"MP", 0, rect.top, "������", "�Ķ���");
		GET_INSTANCE(GraphicEngine)->RenderText(L"50", 145, rect.top - 2, "������", "���");
	}

	{
		D2D1_RECT_F rect;
		rect.left = 55;
		rect.top = 735;
		rect.right = rect.left +745;
		rect.bottom = rect.top + 20;
		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(rect, "��Ȳ��");
		GET_INSTANCE(GraphicEngine)->RenderText(L"EXP", 0, rect.top, "������", "��Ȳ��");
		GET_INSTANCE(GraphicEngine)->RenderText(L"100%", 380, rect.top - 2, "������", "���");
	}

	GET_INSTANCE(UIManager)->Render();
}

void InGameScene::processKeyboardMessage()
{
	ChattingBox* chattingBoxUI = static_cast<ChattingBox*>(GET_INSTANCE(UIManager)->FindUI("ChattingBox"));
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
		if (static_cast<ChattingInputUI*>(chattingBoxUI->FindChildUIs("ChattingInputUI").front())->IsVisible() == false)
		{
			mPlayer->ProcessKeyboardMessage();
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
	}
	else
	{
		mMonsters[id]->SetPosition(x, y);
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

bool InGameScene::checkRange(int x, int y)
{
	if (x < 0 || x > WIDTH - 1 || y < 0 || y > HEIGHT - 1)
	{
		return false;
	}

	return true;
}