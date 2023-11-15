#include "Core.h"
#include "../GraphicEngine/GraphicEngine.h"
#include "../Resource/ResourceManager.h"
#include "../Input/Input.h"
#include "../GameObject/Map/Map.h"
#include "../GameObject/Character/Character.h"
#include "../GameObject/UI/Inventory/Inventory.h"

#include "../Network/Network.h"
#define	 WM_SOCKET WM_USER + 1

INIT_INSTACNE(Core)
Core::Core()
{
	mHandle = nullptr;
	mMaps.clear();
	mPlayer = nullptr;
	mIsReady = false;
}

Core::~Core()
{
	for (auto& tile : mMaps)
	{
		delete tile;
	}

	//mPlayer.reset();
	delete mPlayer;

	mOtherPlayers.clear();
	mMonsters.clear();

	GET_INSTANCE(Camera)->Release();
	GET_INSTANCE(Network)->Release();
	GET_INSTANCE(Input)->Release();
	GET_INSTANCE(ResourceManager)->Release();
	GET_INSTANCE(GraphicEngine)->Release();
}

bool Core::Initialize(HWND handle, int width, int height)
{
	mHandle = handle;
	if(GET_INSTANCE(GraphicEngine)->Initialize(mHandle, width, height) == false)
	{
		return false;
	}

	if (GET_INSTANCE(ResourceManager)->Initialize() == false)
	{
		return false;
	}

	if (GET_INSTANCE(Input)->Initialize() == false)
	{
		return false;
	}

#ifdef SERVER_CONNECT
	if (GET_INSTANCE(Network)->Initialize(handle) == false)
	{
		return false;
	}

	std::string loginId = LOGIN_ID;
	std::string loginPassword = LOGIN_PASSWORD;
	GET_INSTANCE(Network)->SendLoginPacket(loginId, loginPassword);
#else
	if (AddObject() == false)
	{
		return false;
	}
#endif 

	return true;
}

void Core::Run()
{
#ifdef SERVER_CONNECT
	if (mIsReady == false)
	{
		return;
	}
#endif 

	GET_INSTANCE(Input)->ProcessKeyEvent();

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
	}

	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::I_KEY) == true)
	{
		mPlayer->GetInventory()->OpenInventory();
	}

	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::Z_KEY) == true)
	{
		mPlayer->AddItem();
	}

	// render
	GET_INSTANCE(GraphicEngine)->RenderStart();
	for (auto& tile : mMaps)
	{
		tile->Render();
	}

	mPlayer->Render();

	for (auto& player : mOtherPlayers)
	{
		player.second->Render();
	}

	for (auto& monster : mMonsters)
	{
		monster.second->Render();
	}

	GET_INSTANCE(GraphicEngine)->RenderEnd();
}

void Core::Quit()
{
	PostMessage(mHandle, WM_DESTROY, 0, 0);
}

void Core::WindowProc(HWND handle, unsigned int msg, unsigned long long wparam, long long lparam)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		{
			if (wparam == VK_ESCAPE)
			{
				Quit();
			}
			break;
		}

		case WM_SOCKET:
		{
			GET_INSTANCE(Network)->PreocessNetwork(wparam, lparam);
			break;
		}

	default:
		{
			GET_INSTANCE(Input)->ProcessMouseMessage(handle, msg, lparam);
			break;
		}
	}
}

bool Core::AddObject(int myId)
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
	//mPlayer = std::make_shared<Player>();
	if (mPlayer->Initialize(0, 0) == false)
	{
		return false;
	}
	//if (mPlayer->SetTexture("Player") == false)
	//{
	//	return false;
	//}
	mPlayer->Visible();
	mPlayer->SetId(myId);

	// 나를 제외한 나머지 유저 수
	for (int i = 0; i < MAX_CHANNEL_USER; ++i)
	{
		if (i == myId)
		{
			continue;
		}

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
}
