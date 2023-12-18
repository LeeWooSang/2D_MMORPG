#include "SceneManager2.h"
#include "Scene.h"
#include "LoginScene/LoginScene.h"
#include "InGameScene/InGameScene.h"

#include "../GameObject/Character/Character.h"

INIT_INSTACNE(SceneManager2)
SceneManager2::SceneManager2()
{
	mSceneType = SCENE_TYPE2::LOGIN_SCENE;
}

SceneManager2::~SceneManager2()
{
	for (auto& scene : mScenes)
	{
		delete scene.second;
	}
	mScenes.clear();
}

void SceneManager2::Update()
{
	mScenes[mSceneType]->Update();
}

void SceneManager2::Render()
{
	mScenes[mSceneType]->Render();
}

void SceneManager2::AddScene(SCENE_TYPE2 type)
{
	if (type == SCENE_TYPE2::LOGIN_SCENE)
	{
		LoginScene* scene = new LoginScene;
		scene->Initialize();
		mScenes.emplace(type, scene);
	}
	else if (type == SCENE_TYPE2::INGAME_SCENE)
	{
		InGameScene* scene = new InGameScene;
		scene->Initialize();
		mScenes.emplace(type, scene);
	}
}

void SceneManager2::ProcessMouseMessage(unsigned int msg, unsigned long long wParam, long long lParam)
{
	if (mSceneType == SCENE_TYPE2::INGAME_SCENE)
	{
		static_cast<InGameScene*>(mScenes[mSceneType])->GetPlayer()->ProcessMouseMessage(msg, wParam, lParam);
	}
}
