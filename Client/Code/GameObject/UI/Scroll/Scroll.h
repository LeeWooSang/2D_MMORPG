#pragma once
#include "../UI.h"

class Scroll : public UI
{
public:
	Scroll();
	virtual ~Scroll();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

private:
};

class ScrollBar : public UI
{
public:
	ScrollBar();
	virtual ~ScrollBar();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();
};