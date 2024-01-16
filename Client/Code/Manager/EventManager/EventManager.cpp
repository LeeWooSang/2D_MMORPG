#include "EventManager.h"
#include "../SceneMangaer/SceneManager.h"
#include "../../Scene/InGameScene/InGameScene.h"

INIT_INSTACNE(EventManager)
std::queue<std::shared_ptr<PacketBase>> EventManager::mEventQueue;
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
		PacketBase* ev = mEventQueue.front().get();
		switch (ev->packetType)
		{
			case SC_LOGIN_OK:
			{
				scene->InitializeObject(ev->id);
				break;
			}
			case SC_ADD_OBJECT:
			{
				AddPacket* e = static_cast<AddPacket*>(ev);
				scene->AddObject(e->id, e->x, e->y);
				break;
			}
			case SC_REMOVE_OBJECT:
			{
				scene->RemoveObject(ev->id);
				break;
			}
			case SC_POSITION:
			{
				PositionPacket* e = static_cast<PositionPacket*>(ev);
				scene->UpdateObjectPosition(e->id, e->x, e->y);
				break;
			}

			case SC_CHANGE_AVATAR:
			{
				AvatarPacket* e = static_cast<AvatarPacket*>(ev);
				scene->UpdateObjectAvatar(e->id, e->texId);
				break;
			}

			case SC_REQUEST_TRADE:
			{
				scene->RequestTrade();
				break;
			}

			case SC_ADD_TRADE_ITEM:
			{
				AddTradeItemPacket* e = static_cast<AddTradeItemPacket*>(ev);
				scene->AddTradeItem(e->id, e->texId, e->slotNum);
				break;
			}

			case SC_TRADE:
			{
				TradePacket* e = static_cast<TradePacket*>(ev);
				scene->TradeItems(e->id, e->items);
				break;
			}

			case SC_TRADE_POST_PROCESSING:
			{
				scene->TradePostProcessing();
				break;
			}

			case SC_TRADE_CANCEL:
			{
				scene->TradeCancel();
				break;
			}

			default:
				break;
		}

		mEventQueue.front().reset();
		mEventQueue.pop();
	}
}
