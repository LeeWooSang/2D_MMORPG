#pragma once
#include "../Layer.h"

class UI;
class LoginLayer : public Layer
{
public:
	LoginLayer();
	virtual ~LoginLayer();

	virtual bool Initialize();
	virtual void Update();
	virtual void Render();

private:
	UI* mUI;
};