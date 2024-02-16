#include "Core.h"
#include "../Network/Network.h"
#include "../GraphicEngine/GraphicEngine.h"
#include "../GameTimer/GameTimer.h"
#include "../Resource/ResourceManager.h"
#include "../Input/Input.h"
#include "../Manager/EventManager/EventManager.h"
#include "../Manager/SceneMangaer/SceneManager.h"

#define	 WM_SOCKET WM_USER + 1

INIT_INSTACNE(Core)
Core::Core()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(6616);
#endif
	mHandle = nullptr;
}

Core::~Core()
{
	GET_INSTANCE(GameTimer)->Release();
	GET_INSTANCE(EventManager)->Release();
	GET_INSTANCE(SceneManager)->Release();
	GET_INSTANCE(Camera)->Release();
	GET_INSTANCE(Input)->Release();
	GET_INSTANCE(Network)->Release();
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
#endif
	{
		GET_INSTANCE(SceneManager)->AddScene(SCENE_TYPE::LOGIN_SCENE);
		GET_INSTANCE(SceneManager)->AddScene(SCENE_TYPE::SERVER_SCENE);
		GET_INSTANCE(SceneManager)->AddScene(SCENE_TYPE::INGAME_SCENE);
	}

	GET_INSTANCE(SceneManager)->ChangeScene(SCENE_TYPE::LOGIN_SCENE);
	//GET_INSTANCE(SceneManager)->ChangeScene(SCENE_TYPE::SERVER_SCENE);

	GET_INSTANCE(GameTimer)->Reset();

	return true;
}

void Core::Run()
{
	GET_INSTANCE(GameTimer)->Tick();
	float elapsedTime = GET_INSTANCE(GameTimer)->GetElapsedTime();

	GET_INSTANCE(Input)->ProcessKeyEvent();

	GET_INSTANCE(EventManager)->Update();
	GET_INSTANCE(SceneManager)->Update();

	GET_INSTANCE(GraphicEngine)->GetRenderTarget()->BeginDraw();
	GET_INSTANCE(GraphicEngine)->GetRenderTarget()->Clear(D2D1::ColorF(D2D1::ColorF::White));
	GET_INSTANCE(SceneManager)->Render();
	GET_INSTANCE(GraphicEngine)->GetRenderTarget()->EndDraw();

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
		case WM_SOCKET:
		{
			GET_INSTANCE(Network)->PreocessNetwork(wparam, lparam);
			break;
		}

	default:
		{
			GET_INSTANCE(Input)->ProcessWindowMessage(msg, wparam, lparam);
			break;
		}
	}
}

bool Collision(const D2D1_RECT_F& rect1, const D2D1_RECT_F& rect2);
bool Collision(const D2D1_RECT_F& rect1, const D2D1_RECT_F& rect2)
{
	if (rect1.left <= rect2.right && rect1.right >= rect2.left && rect1.bottom >= rect2.top && rect1.top <= rect2.bottom)
	{
		return true;
	}

	return false;
}