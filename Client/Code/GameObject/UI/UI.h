#pragma once
#include "../GameObject.h"

class UI : public GameObject
{
public:
	UI();
	virtual ~UI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

private:
};