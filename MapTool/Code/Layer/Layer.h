#pragma once

class GameObject;
class Layer
{
public:
	Layer();
	virtual ~Layer();

	virtual bool Initialize();
	virtual void Update();
	virtual void Render();

protected:

};