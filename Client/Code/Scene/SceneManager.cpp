#include "SceneManager.h"
#include "Scene.h"
#include "LoginScene/LoginScene.h"
#include "InGameScene/InGameScene.h"
#include "../GameObject/Character/Character.h"

INIT_INSTACNE(SceneManager)
std::queue<PacketEvent> SceneManager::mEventQueue;
SceneManager::SceneManager()
{
	mSceneType = SCENE_TYPE::LOGIN_SCENE;
	mSceneMap.clear();
}

SceneManager::~SceneManager()
{
	for (auto& scene : mSceneMap)
	{
		delete scene.second;
	}
	mSceneMap.clear();
}

void SceneManager::ProcessMouseMessage(unsigned int msg, unsigned long long wParam, long long lParam)
{
	if (mSceneType == INGAME_SCENE)
	{
		static_cast<InGameScene*>(mSceneMap[mSceneType])->GetPlayer()->ProcessMouseMessage(msg, wParam, lParam);
	}
}

void SceneManager::AddScene(SCENE_TYPE type)
{
	if (type == SCENE_TYPE::LOGIN_SCENE)
	{
		Scene* scene = new LoginScene;
		scene->Initialize();
		mSceneMap.emplace(type, scene);
	}

	else if (type == SCENE_TYPE::INGAME_SCENE)
	{
		Scene* scene = new InGameScene;
		scene->Initialize();
		mSceneMap.emplace(type, scene);
	}
}

Scene* SceneManager::FindScene(SCENE_TYPE type)
{
	if (mSceneMap.count(type) == false)
	{
		return nullptr;
	}

	return mSceneMap[type];
}

void SceneManager::Update()
{
	if (mSceneMap[mSceneType]->GetIsReady() == false)
	{
		return;
	}

	processPacketEvent();
	mSceneMap[mSceneType]->Update();
}

void SceneManager::Render()
{
	if (mSceneMap[mSceneType]->GetIsReady() == false)
	{
		return;
	}

	mSceneMap[mSceneType]->Render();
}

void SceneManager::processPacketEvent()
{
	while (mEventQueue.empty() == false)
	{
		PacketEvent& ev = mEventQueue.front();
		InGameScene* scene = static_cast<InGameScene*>(mSceneMap[ev.sceneType]);
		InGamePacket& packet = ev.packet;
		switch (packet.packetType)
		{
		case SC_LOGIN_OK:
		{
			scene->InitializeObject(packet.id);
			break;
		}
		case SC_ADD_OBJECT:
		{
			scene->AddObject(packet.id, packet.x, packet.y);
			break;
		}
		case SC_REMOVE_OBJECT:
		{
			scene->RemoveObject(packet.id);
			break;
		}
		case SC_POSITION:
		{
			scene->UpdateObjectPosition(packet.id, packet.x, packet.y);
			break;
		}

		default:
			break;
		}
		mEventQueue.pop();
	}
}
