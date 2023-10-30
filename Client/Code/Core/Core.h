#pragma once
#include "../Common/Macro.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include "../Common/Defines.h"

class Map;
class Player;
class Character;
class Core
{
	SINGLE_TONE(Core)

public:
	bool Initialize(HWND handle, int width, int height);
	void Run();

	void WindowProc(unsigned int msg, unsigned long long wparam, long long lparam);

	Player* GetPlayer() { return mPlayer.get(); }
	Character* GetOtherPlayer(int id) { return mOtherPlayers[id].get(); }
	Character* GetMonster(int id) { return mMonsters[id].get(); }

private:
	std::vector<Map*> mMaps;
	std::shared_ptr<Player> mPlayer;
	std::unordered_map<int, std::shared_ptr<Character>> mOtherPlayers;
	std::unordered_map<int, std::shared_ptr<Character>> mMonsters;

private:
	HWND mHandle;
};

