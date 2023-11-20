#pragma once
#include "../Common/Macro.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "../Common/Defines.h"

class Map;
class Player;
class Character;
class UI;
class Core
{
	SINGLE_TONE(Core)

public:
	bool Initialize(HWND handle, int width, int height);
	void Run();
	void Quit();

	void WindowProc(HWND handle, unsigned int msg, unsigned long long wParam, long long lParam);

	bool AddObject(int myId = 0);
	//Player* GetPlayer() { return mPlayer.get(); }
	Player* GetPlayer() { return mPlayer; }
	Character* GetOtherPlayer(int id) { return mOtherPlayers[id].get(); }
	Character* GetMonster(int id) { return mMonsters[id].get(); }

	bool GetIsReady()	const { return mIsReady; }

	void render();
private:
	HWND mHandle;

	std::vector<Map*> mMaps;
	//std::shared_ptr<Player> mPlayer;
	Player* mPlayer;
	std::unordered_map<int, std::shared_ptr<Character>> mOtherPlayers;
	std::unordered_map<int, std::shared_ptr<Character>> mMonsters;

	bool mIsReady;
};

