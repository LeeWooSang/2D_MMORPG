#pragma once
#include "../GameObject.h"

constexpr int MAX_ANIMATIONS = 16;   // maximum number of animation sequeces

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

	CHARACTER_STATE_TYPE mState;          // the state of the object (general)
	int mAnimationState;     // an animation state variable, up to you
	int mCurrFrame;     // current animation frame
	int mFrameSize;     // total number of animation frames
	int mCurrAnimation; // index of current animation
	int mAnimationCounter;   // used to time animation transitions
	int mAnimationIndex;     // animation element index
	int mAnimationCountMax; // number of cycles before animation
	int* mAnimations[MAX_ANIMATIONS]; // animation sequences

	wchar_t mMessage[256];
	unsigned long mMessageTime;
};

class Player : public Character
{
public:
	Player();
	virtual ~Player();

	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();
	void Move(char dir);
};