#pragma once
#include "../Scene.h"
#include <vector>
#include <unordered_map>
#include <memory>

class Map;
class Character;
class Player;
class InGameScene : public Scene
{
public:
	InGameScene();
	virtual ~InGameScene();

	virtual bool Initialize();
	virtual void Update();
	virtual void Render();

	void InitializeObject(int myId);
	void AddObject(int id, int x, int y);
	void UpdateObjectPosition(int id, int x, int y);
	void RemoveObject(int id);

	Player* GetPlayer() { return mPlayer; }
	Character* GetOtherPlayer(int id) { return mOtherPlayers[id].get(); }
	Character* GetMonster(int id) { return mMonsters[id].get(); }

private:
	std::vector<Map*> mMaps;
	std::unordered_map<int, std::shared_ptr<Character>> mOtherPlayers;
	std::unordered_map<int, std::shared_ptr<Character>> mMonsters;
	Player* mPlayer;
};
