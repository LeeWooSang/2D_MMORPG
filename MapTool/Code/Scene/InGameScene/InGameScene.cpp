#include "InGameScene.h"
#include "../../../../Server/Code/Common/Protocol.h"

#include "../../Common/Defines.h"
#include "../../Input/Input.h"
#include "../../GameObject/Map/Map.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../GameObject/Character/Character.h"

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
	delete mPlayer;
}

bool InGameScene::Initialize()
{
	for (int i = 0; i < WIDTH; ++i)
	{
		std::vector<std::shared_ptr<Map>> v;
		for (int j = 0; j < HEIGHT; ++j)
		{
			std::shared_ptr<Map> tile = std::make_shared<Map>();
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

	mPlayer->Render();
}

void InGameScene::processKeyboardMessage()
{
	mPlayer->ProcessKeyboardMessage();
}

Player* InGameScene::GetPlayer()
{
	return mPlayer;
}

Monster* InGameScene::GetMonster(int id)
{
	return mMonsters[id].get();
}

bool InGameScene::checkRange(int x, int y)
{
	if (x < 0 || x > WIDTH - 1 || y < 0 || y > HEIGHT - 1)
	{
		return false;
	}

	return true;
}
