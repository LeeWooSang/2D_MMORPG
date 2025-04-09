#include "Script.h"
#include <iostream>

Script::Script()
{
	mLua = nullptr;
}

Script::~Script()
{
	if (mLua != nullptr)
	{
		lua_close(mLua);
		mLua = nullptr;
	}
}

bool Script::Initialize(const std::string& path)
{
	// 루아 객체 생성
	mLua = luaL_newstate();
	if (mLua == nullptr)
	{
		return false;
	}

	luaL_openlibs(mLua);

	int error = luaL_loadfile(mLua, path.c_str());
	if (error)
	{
		showError();
		return false;
	}

	error = lua_pcall(mLua, 0, 0, 0);
	if (error)
	{
		showError();
		return false;
	}

	return true;
}

std::pair<int, int> Script::MonsterMoveAI(int x, int y)
{
	lua_getglobal(mLua, "MoveAI");
	lua_pushnumber(mLua, x);
	lua_pushnumber(mLua, y);
	if (lua_pcall(mLua, 2, 1, 0) != LUA_OK)
	{
		showError();
		return std::make_pair(0, 0);
	}

	if (!lua_istable(mLua, -1))
	{
		showError();
		return std::make_pair(0, 0);
	}

	lua_getfield(mLua, -1, "x");
	int newX = static_cast<int>(lua_tointeger(mLua, -1));
	lua_pop(mLua, 1);

	lua_getfield(mLua, -1, "y");
	int newY = static_cast<int>(lua_tointeger(mLua, -1));
	lua_pop(mLua, 1);

	// 테이블 제거
	lua_pop(mLua, 1);
	return std::make_pair(newX, newY);
}

std::pair<int, int> Script::MonsterChaseTargetStupidAI(int x, int y, int targetX, int targetY)
{
	lua_getglobal(mLua, "ChaseTargetStupidAI");
	lua_pushnumber(mLua, x);
	lua_pushnumber(mLua, y);
	lua_pushnumber(mLua, targetX);
	lua_pushnumber(mLua, targetY);

	if (lua_pcall(mLua, 4, 1, 0) != LUA_OK)
	{
		showError();
		return std::make_pair(0, 0);
	}

	if (!lua_istable(mLua, -1))
	{
		showError();
		return std::make_pair(0, 0);
	}

	lua_getfield(mLua, -1, "x");
	int newX = static_cast<int>(lua_tointeger(mLua, -1));
	lua_pop(mLua, 1);

	lua_getfield(mLua, -1, "y");
	int newY = static_cast<int>(lua_tointeger(mLua, -1));
	lua_pop(mLua, 1);

	// 테이블 제거
	lua_pop(mLua, 1);
	return std::make_pair(newX, newY);
}

void Script::showError()
{
	std::cout << lua_tostring(mLua, -1);
	lua_pop(mLua, 1);
}
