#pragma once
#include "../Common/Macro.h"
#include <unordered_map>

enum class SCENE_TYPE2
{
	LOGIN_SCENE,
	INGAME_SCENE
};

class Scene;
class SceneManager2
{
	SINGLE_TONE(SceneManager2)

public:
	void Update();
	void Render();
	void AddScene(SCENE_TYPE2 type);
	void ChangeScene(SCENE_TYPE2 type) { mSceneType = type; }

	Scene* GetCurScene() { return mScenes[mSceneType]; }
	Scene* FindScene(SCENE_TYPE2 type) { return mScenes[type]; }

	void ProcessMouseMessage(unsigned int msg, unsigned long long wParam, long long lParam);

private:
	SCENE_TYPE2 mSceneType;
	std::unordered_map<SCENE_TYPE2, Scene*> mScenes;
};