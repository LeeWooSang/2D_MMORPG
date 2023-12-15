#include "Scene.h"
#include <iostream>

Scene::Scene()
{
	mIsReady = false;
}

Scene::~Scene()
{
	//for (auto iter = mUIs.begin(); iter != mUIs.end(); )
	//{
	//	if ((*iter) != nullptr)
	//	{
	//		delete (*iter);
	//		(*iter) = nullptr;
	//	}
	//	mUIs.erase(iter);
	//}
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
	// TODO: ���⿡ return ���� �����մϴ�.
}
