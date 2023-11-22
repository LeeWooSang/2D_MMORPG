#pragma once
#include "../GameObject.h"

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

	wchar_t mMessage[256];
	unsigned long mMessageTime;
};

class Inventory;
class ChattingBox;
class Player : public Character
{
public:
	Player();
	virtual ~Player();

	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	void ProcessMouseMessage(unsigned int msg, unsigned long long wParam, long long lParam);

	void Move(char dir);
	void AddItem();
	Inventory* GetInventory() { return mInventory; }
	ChattingBox* GetChattingBox() { return mChattingBox; }
private:
	Inventory* mInventory;
	ChattingBox* mChattingBox;
};