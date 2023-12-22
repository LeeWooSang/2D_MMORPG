#pragma once
#include "../UI.h"

class SkillUI : public UI
{
public:
	SkillUI();
	virtual ~SkillUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOverCollision(int x, int y);

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	void OpenSkillUI();

	void AddSkill(const std::string& name);

private:
	bool mOpen;
};

class SkillSlotUI : public UI
{
public:
	SkillSlotUI();
	virtual ~SkillSlotUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOverCollision(int x, int y);

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	void AddSkill(const std::string& name);

	bool GetEmpty()	const { return mEmpty; }
	void SetEmpty(bool empty) { mEmpty = empty; }

private:
	bool mEmpty;
};