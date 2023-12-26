#pragma once
#include "../GameObject.h"
#include <unordered_map>

enum class SKILL_TYPE
{
	RAGING_BLOW
};

class Animation;
class Skill : public GameObject
{
public:
	Skill();
	virtual ~Skill();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

private:
	SKILL_TYPE mMotion;
	std::unordered_map<SKILL_TYPE, Animation*> mAnimations;
};