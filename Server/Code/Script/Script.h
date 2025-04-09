#pragma once
extern "C"
{
#include "include/lua.h"
#include "include/lauxlib.h"
#include "include/lualib.h"
}
#include <string>

#pragma comment(lib, "../Lib/lua54.lib")
class Script
{
public:
	Script();
	~Script();
	bool Initialize(const std::string& path);
	std::pair<int, int> MonsterMoveAI(int x, int y);
	std::pair<int, int> MonsterChaseTargetStupidAI(int x, int y, int targetX, int targetY);

private:
	void showError();

private:
	lua_State* mLua;
};

