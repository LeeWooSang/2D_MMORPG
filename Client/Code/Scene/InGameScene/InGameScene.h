#pragma once
#include "../Scene.h"
#include <vector>
#include <unordered_map>
#include <memory>

class Map;
class OtherPlayer;
class Player;
class Monster;
class Skill;
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
	void AddPlayer(int id, int x, int y, int* texIds);
	void AddPlayer(struct AddPlayerPacket* packet);

	void AddObject(int id, int x, int y);
	void UpdateObjectPosition(int id, int x, int y, char dir);
	void RemoveObject(int id);
	void RemoveAllObject();

	void UpdateObjectAvatar(int id, int texId);

	void RequestTrade(int id);
	void AddTradeItem(int texId, int slotNum);
	void AddTradeMeso(long long meso);
	void TradeItems(int* items, long long meso);
	void TradeCancel();

	Player* GetPlayer();
	OtherPlayer* GetOtherPlayer(int id);
	Monster* GetMonster(int id);

	void AddSkill(Skill* skill);

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

	std::vector<Map*> mNewTiles;

	std::unordered_map<int, std::shared_ptr<OtherPlayer>> mOtherPlayers;
	std::unordered_map<int, std::shared_ptr<OtherPlayer>> mVisibleOtherPlayers;

	std::unordered_map<int, std::shared_ptr<Monster>> mMonsters;
	std::unordered_map<int, std::shared_ptr<Monster>> mVisibleMonsters;

	Player* mPlayer;

	std::list<Skill*> mSkills;
};
