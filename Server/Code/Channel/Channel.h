#pragma once
#include "../Character/Character.h"
#include <tbb/tbb.h>
#include <shared_mutex>
#include <vector>
#include "../Sector/Sector.h"

constexpr int MAX_CHANNEL_USER = 200;

// 섹터 개수
constexpr int SECTOR_WIDTH_SIZE = WIDTH / SECTOR_WIDTH;
constexpr int SECTOR_HEIGHT_SIZE = HEIGHT / SECTOR_HEIGHT;

class Channel
{
public:
	Channel();
	~Channel();
	bool Initialize(int channel);

	bool IsFull();
	int PushUser(int id);
	void PopUser(int index) { mUserIdList.erase(index); }
	int FindUser(int index)	const;

	void PushSectorObject(int x, int y, int id);
	int FindSectorObjectIndex(int x, int y, int id);
	int FindSectorObjectIndexById(int sectorXId, int sectorYId, int id);

	std::vector<int> GetUserIds();

	Sector** GetSectors() { return mSectors; }
	Sector& FindSector(int x, int y);
	//Sector** FindSector(int x, int y);
	Sector& FindSectorById(int sectorXId, int sectorYId) { return mSectors[sectorXId][sectorYId]; }

private:

	std::shared_mutex mChannelMtx;

	// 인덱스, 유저 id
	tbb::concurrent_hash_map<int, int> mUserIdList;

	Sector** mSectors;

	//std::array<std::array<int, HEIGHT>, WIDTH> mMaps;
	
	//Sector* mSecotors;
	//Sector mSectors[SECTOR_WIDTH_SIZE][SECTOR_HEIGHT_SIZE];
	//std::array<std::array<Sector, SECTOR_HEIGHT_SIZE>, SECTOR_WIDTH_SIZE> mSectors;
};