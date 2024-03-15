#pragma once
#include <iostream>

class Object
{
public:
	Object();
	virtual ~Object();

	virtual bool Initialize(int x, int y) = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
};

#define BITMAP_STATE_DEAD	0
#define BITMAP_STATE_ALIVE   1
#define BITMAP_STATE_DYING   2 
#define BITMAP_ATTR_LOADED   128

enum ATTR_STATE_TYPE
{
	DEFAULT			= 0x0000,		//	0000 0000
	VISIBLE				= 0x0001,		// 0000 0001
};

class Texture;
class GameObject : Object
{
public:
	GameObject();
	virtual ~GameObject();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	void Reset();

	virtual void SetPosition(int x, int y) { mPos = std::make_pair(x, y); }
	const std::pair<int, int>& GetPosition()	const { return mPos; }
	char GetDirection()	const { return mDir; }

	Texture* GetTexture()	const { return mTexture; }
	bool SetTexture(const std::string& name);

	virtual bool CheckDistance(int x, int y);
	virtual void Visible() { mAttr |= ATTR_STATE_TYPE::VISIBLE; }
	virtual void NotVisible() { mAttr &= ~ATTR_STATE_TYPE::VISIBLE; }
	bool IsVisible();

	virtual void SetRect(int width, int height) { mRect = std::make_pair(width, height); }

protected:
	// x, y
	std::pair<int, int> mPos;
	std::pair<float, float> mWorldPos;
	std::pair<int, int> mRect;

	char mDir;
	int mAttr;
	Texture* mTexture;
};

