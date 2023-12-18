#pragma once
#include "../../Common/Macro.h"
#include <unordered_map>

enum class SCENE_TYPE
{
	LOGIN_SCENE,
	INGAME_SCENE
};

class Scene;
class SceneManager
{
	SINGLE_TONE(SceneManager)

public:
	void Update();
	void Render();
	void AddScene(SCENE_TYPE type);
	void ChangeScene(SCENE_TYPE type) { mSceneType = type; }

	Scene* GetCurScene();
	Scene* FindScene(SCENE_TYPE type);

	void ProcessMouseMessage(unsigned int msg, unsigned long long wParam, long long lParam);

private:
	SCENE_TYPE mSceneType;
	std::unordered_map<SCENE_TYPE, Scene*> mScenes;
};