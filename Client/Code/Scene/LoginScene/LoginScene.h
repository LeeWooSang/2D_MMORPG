#pragma once
#include "../Scene.h"

class UI;
class LoginScene : public Scene
{
public:
	LoginScene();
	virtual ~LoginScene();

	virtual bool Initialize();
	virtual void Update();
	virtual void Render();

	virtual void processKeyboardMessage();

private:
	UI* mInputUI;
};

void LoginClick(const std::string& name);