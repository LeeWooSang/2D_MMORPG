#pragma once
#include "../GameObject.h"
#include <unordered_map>
#include <vector>

enum class SKILL_TYPE
{
	RAGING_BLOW
};

class Skill : public GameObject
{
public:
	Skill();
	virtual ~Skill();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	void AddEffect(int texId);
	void UseSkill();

	const std::string& GetName()	 const { return mName; }
	void SetName(const std::string& name) { mName = name; }

	int GetSkillLevel()	const { return mSkillLevel; }
	void SetSkillLevel(int skillLevel) { mSkillLevel = skillLevel; }


private:
	bool checkFinish();

private:
	std::string mName;
	int mSkillLevel;

	int mCurrentEffect;
	std::vector<class SkillEffect*> mEffects;
};

class Animation;
class SkillEffect : public GameObject
{
public:
	SkillEffect();
	virtual ~SkillEffect();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();
;
	void AddAnimation(const std::string& texName);
	Animation* GetAnimation() { return mAnimation; }

private:
	Animation* mAnimation;
};