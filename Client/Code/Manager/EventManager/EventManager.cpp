#include "EventManager.h"
#include "../SceneMangaer/SceneManager.h"
#include "../../Scene/InGameScene/InGameScene.h"

INIT_INSTACNE(EventManager)
std::queue<PacketEvent> EventManager::mEventQueue;
EventManager::EventManager()
{
}

EventManager::~EventManager()
{
}

void EventManager::Update()
{
	processPacketEvent();
}

void EventManager::processPacketEvent()
{
	while (mEventQueue.empty() == false)
	{
		InGameScene* scene = static_cast<InGameScene*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE));
		PacketEvent& ev = mEventQueue.front();
		switch (ev.packetType)
		{
			case SC_LOGIN_OK:
			{
				scene->InitializeObject(ev.id);
				break;
			}
			case SC_ADD_OBJECT:
			{
				scene->AddObject(ev.id, ev.x, ev.y);
				break;
			}
			case SC_REMOVE_OBJECT:
			{
				scene->RemoveObject(ev.id);
				break;
			}
			case SC_POSITION:
			{
				scene->UpdateObjectPosition(ev.id, ev.x, ev.y);
				break;
			}

			case SC_CHANGE_AVATAR:
			{
				scene->UpdateObjectAvatar(ev.id, 0);
				break;
			}

			default:
				break;
		}
		mEventQueue.pop();
	}
}
