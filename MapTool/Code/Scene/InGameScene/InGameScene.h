#pragma once
#include "../Scene.h"
#include <vector>
#include <unordered_map>
#include <memory>

class Map;
class Player;
class Monster;
class InGameScene : public Scene
{
public:
	InGameScene();
	virtual ~InGameScene();

	virtual bool Initialize();
	virtual void Update();
	virtual void Render();
	virtual void processKeyboardMessage();

	Player* GetPlayer();
	Monster* GetMonster(int id);

private:
	bool checkRange(int x, int y);

private:
	std::pair<int, int> mTileMinPos;
	std::pair<int, int> mTileMaxPos;
	std::vector<std::vector<std::shared_ptr<Map>>> mTiles;

	std::vector<Map*> mNewTiles;

	std::unordered_map<int, std::shared_ptr<Monster>> mMonsters;
	std::unordered_map<int, std::shared_ptr<Monster>> mVisibleMonsters;

	Player* mPlayer;
};
