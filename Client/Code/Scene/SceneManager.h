#pragma once
#include "../Common/Macro.h"
#include "../../../Server/Code/Common/Protocol.h"
#include <unordered_map>
#include <mutex>
#include <queue>

#include "Scene.h"
enum SCENE_TYPE
{
	LOGIN_SCENE,
	INGAME_SCENE
};

struct InGamePacket
{
	char packetType;
	int id;
	int x;
	int y;
};

struct PacketEvent
{
	PacketEvent(SCENE_TYPE _sceneType, InGamePacket _packet)
		: sceneType(_sceneType), packet(_packet) {}

	SCENE_TYPE sceneType;
	InGamePacket packet;
};

class Scene;
class SceneManager
{
	SINGLE_TONE(SceneManager)

public:
	void ProcessMouseMessage(unsigned int msg, unsigned long long wParam, long long lParam);

	void ChangeScene(SCENE_TYPE type) { mSceneType = type; }
	void AddScene(SCENE_TYPE type);
	void AddScene(SCENE_TYPE type, Scene* scene) { mSceneMap.emplace(type, scene); }

	SCENE_TYPE GetCurSceneType()	const { return mSceneType; }
	//Scene* GetCurScene() { return mSceneMap[mSceneType]; }
	Scene* GetCurScene();
	Scene* FindScene(SCENE_TYPE type);

	void AddPacketEvent(SCENE_TYPE sceneType, InGamePacket packet) { mEventQueue.push(PacketEvent(sceneType, packet)); }

	void Update();
	void Render();

private:
	void processPacketEvent();

private:
	SCENE_TYPE mSceneType;
	std::unordered_map<SCENE_TYPE, Scene*> mSceneMap;

	std::mutex mEventQueueMtx;
	static std::queue<PacketEvent> mEventQueue;
};