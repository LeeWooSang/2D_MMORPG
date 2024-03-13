#pragma once
#include "../GameObject.h"
#include <unordered_map>
#include <list>
#include <string>

enum class CHARACTER_STATE_TYPE
{
	DEFAULT = 0x0000,	//	0000 0000
	LIVE = 0x0001,			// 0000 0001 
	DEAD = 0x0002		// 0000 0010	
};

class Character : public GameObject
{
public:
	Character();
	virtual ~Character();

	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();
	void Reset();

	int GetId()	const { return mId; }
	void SetId(int id) { mId = id; }
	
protected:
	int mId;

	CHARACTER_STATE_TYPE mState;
};

enum class ANIMATION_MOTION_TYPE
{
	IDLE,
	WALK,
	JUMP
};

class Animation;
class AnimationCharacter : public GameObject
{
public:
	AnimationCharacter();
	virtual ~AnimationCharacter();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();
	void AvatarRender(int x, int y);

	virtual void SetPosition(int x, int y);
	virtual void SetDirection(char dir);

	virtual void Visible();
	virtual void NotVisible();

	int GetId()	const { return mId; }
	void SetId(int id) { mId = id; }

	void AddChild(const std::string& name, AnimationCharacter* obj);
	AnimationCharacter* FindChildObject(const std::string& name);

	void AddAnimation(ANIMATION_MOTION_TYPE motion, Animation* animation)		{ mAnimations.emplace(motion, animation); }
	std::unordered_map<ANIMATION_MOTION_TYPE, Animation*>& GetAnimations() { return mAnimations; }
	
	void SetAnimationMotion(ANIMATION_MOTION_TYPE motion);
	void SetAvatar(const std::string& parts, ANIMATION_MOTION_TYPE motion, const std::string& objName, int count);

	std::vector<int>& GetAvatarIds() { return mAvatarIds; }
	void SetAvatarId(int texId);
	void RemoveAvatarId(int texId);

	void SetAvatar(int texId);

	void TakeOffAvatar(int texId);

protected:
	int mId;
	std::unordered_map<std::string, AnimationCharacter*> mChildObjects;
	std::list<AnimationCharacter*> mRenderChildObjects;

	AnimationCharacter* mParent;

	ANIMATION_MOTION_TYPE mMotion;
	std::unordered_map<ANIMATION_MOTION_TYPE, Animation*> mAnimations;

	std::vector<int> mAvatarIds;
};

class Player : public AnimationCharacter
{
public:
	Player();
	virtual ~Player();

	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	void ProcessKeyboardMessage();
	void ProcessMouseMessage(unsigned int msg, unsigned long long wParam, long long lParam);

	void Move(char dir);
	void AddItem();

private:
	float mElapsedTime;
	bool mFlag;
};

class OtherPlayer : public AnimationCharacter
{
public:
	OtherPlayer();
	virtual ~OtherPlayer();

	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

private:
};

class Monster : public AnimationCharacter
{
public:
	Monster();
	virtual ~Monster();

	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

private:
	float mElapsedTime;
};