#pragma once
#include "../GameObject.h"

class Texture;
class Map : public GameObject
{
public:
	Map();
	virtual ~Map();

	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	bool CheckMouseOver(int mouseX, int mouseY);
	bool CheckMouseClick(int mouseX, int mouseY);

private:
	std::pair<int, int> mRect;
	bool mMouseOver;
	bool mMouseDown;
	bool mMouseUp;
	bool mMouseClick;
};
