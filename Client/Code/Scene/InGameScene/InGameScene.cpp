#include "InGameScene.h"
#include "../../Common/Defines.h"
#include "../../Input/Input.h"
#include "../../GameObject/Map/Map.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../GameObject/Character/Character.h"
#include "../../GameObject/UI/UIManager.h"
#include "../../../../Server/Code/Common/Protocol.h"

InGameScene::InGameScene()
	: Scene()
{
	mPlayer = nullptr;
	mIsReady = false;
}

InGameScene::~InGameScene()
{
	mOtherPlayers.clear();
	mMonsters.clear();

	for (auto& tile : mMaps)
	{
		delete tile;
	}

	//mPlayer.reset();
	delete mPlayer;
}

bool InGameScene::Initialize()
{
	mMaps.reserve(WIDTH * HEIGHT);

	for (int i = 0; i < WIDTH; ++i)
	{
		for (int j = 0; j < HEIGHT; ++j)
		{
			Map* map = new Map;
			if (map->Initialize(i, j) == false)
			{
				return false;
			}

			if (map->SetTexture("Tile") == false)
			{
				return false;
			}
			mMaps.emplace_back(map);
}
	}

	mPlayer = new Player;
	if (mPlayer->Initialize(0, 0) == false)
	{
		return false;
	}
	mPlayer->Visible();
	mPlayer->SetId(-1);

	// 나를 제외한 나머지 유저 수
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

	// 몬스터
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

	mIsReady = true;

	return true;
}

void InGameScene::Update()
{	
	if (mIsReady == false)
	{
		return;
	}

	// update
	mPlayer->Update();

	for (auto& tile : mMaps)
	{
		if (tile->CheckDistance(mPlayer->GetPosition().first, mPlayer->GetPosition().second) == true)
		{
			tile->Visible();
		}
		else
		{
			tile->NotVisible();
		}

		tile->Update();
	}

	for (auto& otherPlayer : mOtherPlayers)
	{
		otherPlayer.second->Update();
	}

	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::Z_KEY) == true)
	{
		mPlayer->AddItem();
	}

	GET_INSTANCE(UIManager)->Update();
}

void InGameScene::Render()
{
	if (mIsReady == false)
	{
		return;
	}

	for (auto& tile : mMaps)
	{
		tile->Render();
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

	GET_INSTANCE(UIManager)->Render();
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
