#include "Scene.h"
#include <iostream>
#include "../Core/Core.h"

void GameQuitClick(const std::string& name)
{
	GET_INSTANCE(Core)->Quit();
}

Scene::Scene()
{
	mIsReady = false;
}

Scene::~Scene()
{
}

bool Scene::Initialize()
{
	return false;
}

void Scene::Update()
{
}

void Scene::Render()
{
}

void Scene::processKeyboardMessage()
{
}

void Scene::AddSceneUI(const std::string& name, UI* ui)
{
	mUIs.emplace_back(ui);
	mUIsMap.emplace(name, ui);
}

UI* Scene::FindUI(const std::string& name)
{
	if (mUIsMap.count(name) == true)
	{
		return mUIsMap[name];
	}

	return nullptr;
}

std::list<UI*>& Scene::GetSceneUIs()
{
	return mUIs;
}
