#pragma once
class SceneBase
{
public:
	SceneBase() {}
	virtual ~SceneBase() {};

	virtual bool Initialize() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;

	virtual void processKeyboardMessage() = 0;
};

#include <list>
#include <unordered_map>
#include <string>
class UI;
class Scene : public SceneBase
{
public:
	Scene();
	virtual ~Scene();

	virtual bool Initialize();
	virtual void Update();
	virtual void Render();

	virtual void processKeyboardMessage();

	bool GetIsReady()	const { return mIsReady; }

protected:
	bool mIsReady;
};

void GameQuitClick(const std::string& name);