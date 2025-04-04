#include "SceneManager.h"
#include <Windows.h>

#include "../../Scene/Scene.h"
#include "../../Scene/LoginScene/LoginScene.h"
#include "../../Scene/ServerScene/ServerScene.h"
#include "../../Scene/InGameScene/InGameScene.h"

#include "../UIManager/UIManager.h"
#include "../../GameObject/UI/Inventory/Inventory.h"

INIT_INSTACNE(SceneManager)
SceneManager::SceneManager()
{
	mSceneType = SCENE_TYPE::LOGIN_SCENE;
}

SceneManager::~SceneManager()
{
	GET_INSTANCE(UIManager)->Release();

	for (auto& scene : mScenes)
	{
		delete scene.second;
	}
	mScenes.clear();
}

void SceneManager::Update()
{
	mScenes[mSceneType]->Update();
}

void SceneManager::Render()
{
	mScenes[mSceneType]->Render();
}

void SceneManager::AddScene(SCENE_TYPE type)
{
	Scene* scene = nullptr;

	switch (type)
	{
		case SCENE_TYPE::LOGIN_SCENE:
		{
			scene = new LoginScene;
			break;
		}

		case SCENE_TYPE::SERVER_SCENE:
		{
			scene = new ServerScene;
			break;
		}

		case SCENE_TYPE::INGAME_SCENE:
		{
			scene = new InGameScene;
			break;
		}

		default:
			break;
	}

	mScenes.emplace(type, scene);
	scene->Initialize();
}

Scene* SceneManager::GetCurScene()
{
	return mScenes[mSceneType];
}

Scene* SceneManager::FindScene(SCENE_TYPE type)
{
	return mScenes[type];
}

void SceneManager::ProcessMouseMessage(unsigned int msg, unsigned long long wParam, long long lParam)
{
	if (mSceneType != SCENE_TYPE::INGAME_SCENE)
	{
		return;
	}
	InGameScene* scene = static_cast<InGameScene*>(mScenes[mSceneType]);
	Inventory* inventory = static_cast<Inventory*>(scene->FindUI("Inventory"));

	switch (msg)
	{
		case WM_MOUSEWHEEL:
		{
			inventory->ProcessMouseWheelEvent(wParam);
			break;
		}
		case WM_LBUTTONDBLCLK:
		{
			inventory->ProcessMouseDoubleClickEvent();
			break;
		}
		default:
			break;
	}

}
