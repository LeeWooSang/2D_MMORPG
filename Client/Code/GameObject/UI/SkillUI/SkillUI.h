#pragma once
#include "../UI.h"

class Skill;
class SkillUI : public UI
{
public:
	SkillUI();
	virtual ~SkillUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	void OpenSkillUI();

	void AddSkillSlot(const std::string& name, int texId);
	Skill* FindSkill(const std::string& name);

	int GetSkillPoint()	const { return mSkillPoint; }
	void SetSkillPoint(int skillPoint) { mSkillPoint = skillPoint; }

private:
	bool mOpen;
	int mSkillPoint;
};

class SkillSlotUI : public UI
{
public:
	SkillSlotUI();
	virtual ~SkillSlotUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	void AddSkill(const std::string& name, int texId);

	bool GetEmpty()	const { return mEmpty; }
	void SetEmpty(bool empty) { mEmpty = empty; }

	Skill* GetSkill() { return mSkill; }

private:
	bool mEmpty;
	class Skill* mSkill;
};