#pragma once
#include <unordered_map>
#include <string>

class UI;
class Layer;

class Test22
{
public:
	~Test22();
	virtual bool Initialize() { return true; };
	virtual void Update() {};
	virtual void Render() {};
	void Add(const std::string& name, UI* ui) { mUIs.emplace(name, ui); };

protected:
	std::unordered_map<std::string, UI*> mUIs;
};
class Test2 : public Test22
{
public:
	~Test2();
	virtual bool Initialize();
	virtual void Update();
	virtual void Render();

private:
	virtual void processKeyboardMessage();

private:
};