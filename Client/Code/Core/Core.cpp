#include "Core.h"
#include "../Network/Network.h"
#include "../GraphicEngine/GraphicEngine.h"
#include "../GameTimer/GameTimer.h"
#include "../Resource/ResourceManager.h"
#include "../Input/Input.h"

#include "../GameObject/Map/Map.h"
#include "../GameObject/Character/Character.h"
#include "../GameObject/UI/Inventory/Inventory.h"
#include "../GameObject/UI/ChattingBox/ChattingBox.h"

#include "../Resource/ResourceManager.h"
#include "../Resource/Texture/Texture.h"

#include "../GameObject/UI/UIManager.h"

#define	 WM_SOCKET WM_USER + 1

INIT_INSTACNE(Core)
bool Collision(const D2D1_RECT_F& rect1, const D2D1_RECT_F& rect2);
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
	GET_INSTANCE(GameTimer)->Release();
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
	GET_INSTANCE(GameTimer)->Reset();

	return true;
}

void Core::Run()
{
	GET_INSTANCE(GameTimer)->Tick();
	float elapsedTime = GET_INSTANCE(GameTimer)->GetElapsedTime();

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

	GET_INSTANCE(UIManager)->Update();
	//if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::ENTER_KEY) == true)
	//{
	//	mPlayer->GetChattingBox()->OpenChattingBox();
	//	//mChattingBox->OpenChattingBox();
	//}

	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::Z_KEY) == true)
	{
		mPlayer->AddItem();
	}

	render();

	std::wstring title = L"MapleStory " + std::to_wstring(GET_INSTANCE(GameTimer)->GetFrameRate()) + L" FPS";
	::SetWindowText(mHandle, const_cast<wchar_t*>(title.c_str()));
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
			GET_INSTANCE(Input)->ProcessMouseMessage(msg, wparam, lparam);
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

	return true;
}

void Core::render()
{
	GET_INSTANCE(GraphicEngine)->GetRenderTarget()->BeginDraw();
	GET_INSTANCE(GraphicEngine)->GetRenderTarget()->Clear(D2D1::ColorF(D2D1::ColorF::Black));

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

	//std::pair<float, float> tilePos = std::make_pair(350.0, 350.0);
	//static std::pair<float, float> boxPos = std::make_pair(350.0, 0.0);
	//static std::pair<float, float> boxPos2 = std::make_pair(600.0, 350.0);
	//static std::pair<float, float> boxPos3 = std::make_pair(0.0, 350.0);
	//static std::pair<float, float> boxPos4 = std::make_pair(350.0, 750.0);
	//{
	//	float gravity = 9.8;
	//	float speed = 0.2;

	//	D2D1_RECT_F rect1;
	//	rect1.left = tilePos.first;
	//	rect1.top = tilePos.second;
	//	rect1.right = rect1.left + 100;
	//	rect1.bottom = rect1.top + 100;

	//	D2D1_RECT_F rect2;
	//	rect2.left = boxPos.first;
	//	rect2.top = boxPos.second;
	//	rect2.right = rect2.left + 60;
	//	rect2.bottom = rect2.top + 60;

	//	D2D1_RECT_F rect3;
	//	rect3.left = boxPos2.first;
	//	rect3.top = boxPos2.second;
	//	rect3.right = rect3.left + 60;
	//	rect3.bottom = rect3.top + 60;

	//	D2D1_RECT_F rect4;
	//	rect4.left = boxPos3.first;
	//	rect4.top = boxPos3.second;
	//	rect4.right = rect4.left + 60;
	//	rect4.bottom = rect4.top + 60;

	//	D2D1_RECT_F rect5;
	//	rect5.left = boxPos4.first;
	//	rect5.top = boxPos4.second;
	//	rect5.right = rect5.left + 60;
	//	rect5.bottom = rect5.top + 60;

	//	if (Collision(rect2, rect1) == false && rect2.bottom < WINDOW_HEIGHT)
	//	{
	//		boxPos.second += gravity * speed;
	//	}
	//	if (Collision(rect3, rect1) == false && rect3.left >= 0)
	//	{
	//		boxPos2.first -= gravity * speed;
	//	}
	//	if (Collision(rect4, rect1) == false && rect4.left < WINDOW_WIDTH)
	//	{
	//		boxPos3.first += gravity * speed;
	//	}
	//	if (Collision(rect5, rect1) == false && rect5.top >= 0)
	//	{
	//		boxPos4.second -= gravity * speed;
	//	}
	//	GET_INSTANCE(GraphicEngine)->RenderRectangle(rect1, "흰색");
	//	GET_INSTANCE(GraphicEngine)->RenderRectangle(rect2);		
	//	GET_INSTANCE(GraphicEngine)->RenderRectangle(rect3);
	//	GET_INSTANCE(GraphicEngine)->RenderRectangle(rect4);
	//	GET_INSTANCE(GraphicEngine)->RenderRectangle(rect5);
	//}


	GET_INSTANCE(GraphicEngine)->GetRenderTarget()->EndDraw();
}

bool Collision(const D2D1_RECT_F& rect1, const D2D1_RECT_F& rect2)
{
	if (rect1.left <= rect2.right && rect1.right >= rect2.left && rect1.bottom >= rect2.top && rect1.top <= rect2.bottom)
	{
		return true;
	}

	return false;
}