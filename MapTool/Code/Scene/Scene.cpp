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