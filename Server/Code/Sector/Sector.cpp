#include "Sector.h"
#include <iostream>

Sector::Sector()
{
	mObjectIds.clear();
	mMonsters = nullptr;

	mObjectStartId = 0;
	mObjectEndId = 0;
}

Sector::~Sector()
{
	delete[] mMonsters;
}

bool Sector::Initialize(int channel, int monsterId, std::pair<int, int> sectorId, int x, int y)
{
	// 몬스터는 미리 넣어놓는다.
	mMonsters = new Monster[SECTOR_MAX_MONSTER];
	for (int i = 0; i < SECTOR_MAX_MONSTER; ++i)
	{
		int id = monsterId + i;
		if (mMonsters[i].Inititalize(id) == false)
		{
			return false;
		}

		std::random_device rd;
		std::default_random_engine dre(rd());
		std::uniform_int_distribution<int> uidX(x, x + SECTOR_WIDTH - 1);
		std::uniform_int_distribution<int> uidY(y, y + SECTOR_HEIGHT - 1);
		int posX = uidX(dre);
		int posY = uidY(dre);

		mMonsters[i].SetRange(std::make_pair(x, y), std::make_pair(x + SECTOR_WIDTH - 1, y + SECTOR_HEIGHT - 1));
		mMonsters[i].SetPosition(posX, posY);

		mMonsters[i].SetChannel(channel);
		mMonsters[i].SetSectorId(sectorId);
		
		mObjectIds.insert(std::make_pair(id, i));
	}

	return true;
}

void Sector::PushObject(int id)
{
	tbb::concurrent_hash_map<int, int>::accessor acc;
	mObjectIds.insert(acc, id);
	acc->second = id;
	acc.release();
}

int Sector::GetObjectIndex(int id) const
{
	tbb::concurrent_hash_map<int, int>::const_accessor acc;
	if (mObjectIds.find(acc, id))
	{
		return acc->second;
	}
	return -1;
}
