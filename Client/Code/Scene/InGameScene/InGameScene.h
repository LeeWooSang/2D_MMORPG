#pragma once
#include "../Scene.h"
#include <vector>
#include <unordered_map>
#include <memory>

class Map;
class Character;
class AnimationCharacter;
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

	void InitializeObject(int myId);
	void AddObject(int id, int x, int y);
	void UpdateObjectPosition(int id, int x, int y);
	void RemoveObject(int id);

	Player* GetPlayer() { return mPlayer; }
	Character* GetOtherPlayer(int id) { return mOtherPlayers[id].get(); }
	Monster* GetMonster(int id) { return mMonsters[id].get(); }

	void SetAvatarPose0(int x, int y);
	void SetAvatarPose1(int x, int y);
	void SetAvatarPose2(int x, int y);
	void SetAvatarPose3(int x, int y);

	void SetMonsterPose0(int x, int y);
	void SetMonsterPose1(int x, int y);

private:
	bool checkRange(int x, int y);

private:
	std::pair<int, int> mTileMinPos;
	std::pair<int, int> mTileMaxPos;
	std::vector<std::vector<Map*>> mTiles;

	std::unordered_map<int, std::shared_ptr<Character>> mOtherPlayers;
	std::unordered_map<int, std::shared_ptr<Monster>> mMonsters;

	Player* mPlayer;

	std::vector<AnimationCharacter*> mNPCs;
	//std::vector<AnimationCharacter*> mMobs;

};
