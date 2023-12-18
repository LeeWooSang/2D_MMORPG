#include "Scene.h"
#include <iostream>

Scene::Scene()
{
	mIsReady = false;
}

Scene::~Scene()
{
	for (auto& ui : mUIs)
	{
		delete ui;
	}
	mUIs.clear();
	mUIsMap.clear();
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
	// TODO: 여기에 return 문을 삽입합니다.
}
