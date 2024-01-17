#pragma once
#include "../Scene.h"
#include <vector>
#include <unordered_map>
#include <memory>

class Map;
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
	void UpdateObjectAvatar(int id, int texId);

	void RequestTrade(int id);
	void AddTradeItem(int id, int texId, int slotNum);
	void TradeItems(int id, int* items);
	void TradePostProcessing();
	void TradeCancel();

	Player* GetPlayer();
	AnimationCharacter* GetOtherPlayer(int id);
	Monster* GetMonster(int id);

	void SetAvatarPose0(int x, int y);
	void SetAvatarPose1(int x, int y);
	void SetAvatarPose2(int x, int y);
	void SetAvatarPose3(int x, int y);

	void SetMonsterPose0(int x, int y);
	void SetMonsterPose1(int x, int y);
	void SetInventory(int x, int y);
	void SetSkillUI(int x, int y);
	void SetSkillEffect(int x, int y);

private:
	bool checkRange(int x, int y);

private:
	std::pair<int, int> mTileMinPos;
	std::pair<int, int> mTileMaxPos;
	std::vector<std::vector<Map*>> mTiles;

	std::unordered_map<int, std::shared_ptr<AnimationCharacter>> mOtherPlayers;
	std::unordered_map<int, std::shared_ptr<Monster>> mMonsters;

	Player* mPlayer;
};
