#pragma once
#include "../GameObject.h"
#include <vector>
#include <unordered_map>
#include <string>

class UI : public GameObject
{
public:
	UI();
	virtual ~UI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();
	virtual void Move(int mouseX, int mouseY);
	virtual void Click(int mouseX, int mouseY);

	std::pair<int, int> GetOriginPosition()	const { return std::make_pair(mOriginX, mOriginY); }
	void SetOriginPosition(int x, int y) { mOriginX = x;	mOriginY = y; }

	virtual void SetPosition(int x, int y);
	
	bool Collision(int x, int y);
	virtual void MouseOverCollision(int x, int y);

	virtual bool CheckContain(int left, int top, int right, int bottom);

	UI* GetParentUI() { return mParentUI; }
	void AddChildUI(std::string key, UI* ui);
	std::vector<UI*>& FindChildUIs(const std::string& key) { return mChildUIs[key]; }
	std::unordered_map<std::string, std::vector<UI*>>& GetChildUIs();

	bool GetMouseOver()	const;
	void SetMouseOver(bool mouseOver);

	bool GetMouseLButtonDown()	const;
	void SetMouseLButtonDown(bool lButtonDown);

	bool GetMouseLButtonClick() const { return mMouseLButtonClick; }
	void SetMouseLButtonClick(bool click) { mMouseLButtonClick = click; }

protected:
	UI* mParentUI;
	std::unordered_map<std::string, std::vector<UI*>> mChildUIs;
	std::vector<std::vector<UI*>> mRenderChildUIs;
	int mRenderIndex;

	int mOriginX;
	int mOriginY;

	bool mMouseOver;
	bool mMouseLButtonDown;
	bool mMouseLButtonClick;

	std::pair<int, int> mDragStartPos;
};