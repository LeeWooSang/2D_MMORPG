#pragma once
#include "../GameObject.h"
#include <unordered_map>
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
	void SetAnimationInfo(int frameSize);
	
protected:
	int mId;

	CHARACTER_STATE_TYPE mState;
};

class Inventory;
class Player : public Character
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

class AnimationCharacter : public GameObject
{
public:
	AnimationCharacter();
	virtual ~AnimationCharacter();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void SetPosition(int x, int y);
	void SetOriginPos(int x, int y) { mOriginX = x; mOriginY = y; }

	void AddChild(const std::string& name, AnimationCharacter* obj);

private:
	std::unordered_map<std::string, AnimationCharacter*> mChildObjects;
	AnimationCharacter* mParent;
	int mOriginX;
	int mOriginY;
};