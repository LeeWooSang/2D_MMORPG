#pragma once
#include "../GameObject.h"
#include <vector>

class UI : public GameObject
{
public:
	UI();
	virtual ~UI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void SetPosition(int x, int y);

	bool Collision(int x, int y);

	UI* GetParentUI() { return mParentUI; }
	void AddChildUI(UI* ui);

protected:
	UI* mParentUI;
	std::vector<UI*> mChildUIs;

	int mOriginX;
	int mOriginY;
};