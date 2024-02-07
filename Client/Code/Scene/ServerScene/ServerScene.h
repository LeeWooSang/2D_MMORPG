#pragma once
#include "../Scene.h"

class ServerScene : public Scene
{
public:
	ServerScene();
	virtual ~ServerScene();

	virtual bool Initialize();
	virtual void Update();
	virtual void Render();

	virtual void processKeyboardMessage();

private:
};