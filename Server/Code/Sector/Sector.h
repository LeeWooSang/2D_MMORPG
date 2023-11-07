#pragma once
#include "../Common/Protocol.h"
#include <array>
#include <tbb/tbb.h>
#include "../Character/Character.h"

// 섹터 크기
constexpr int SECTOR_WIDTH = WIDTH / 10;
constexpr int SECTOR_HEIGHT = HEIGHT / 10;

// 섹터 개수
constexpr int SECTOR_WIDTH_SIZE = WIDTH / SECTOR_WIDTH;
constexpr int SECTOR_HEIGHT_SIZE = HEIGHT / SECTOR_HEIGHT;

class Monster;
class Sector
{
public:
	Sector();
	~Sector();

	bool Initialize(int channel, int monsterId, std::pair<int, int> sectorId, int x, int y);

	void PushObject(int id);
	void PopObject(int id) { mObjectIds.erase(id); }

	int GetObjectIndex(int id) const;

	Monster* GetMonsters() { return mMonsters; }
	Monster& GetMonster(int index) { return mMonsters[index]; }

	tbb::concurrent_hash_map<int, int>& GetObjectIds() { return mObjectIds; }

	void SetObjectStartEndId(int start, int end) { mObjectStartId = start; mObjectEndId = end; }
	int GetStartId()	const { return mObjectStartId; }
	int GetEndId()	const { return mObjectEndId; }

private:
	tbb::concurrent_hash_map<int, int> mObjectIds;
	Monster* mMonsters;

	int mObjectStartId;
	int mObjectEndId;
};