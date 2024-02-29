#include "Channel.h"
#include "../Character/Character.h"

Channel::Channel()
{
	//for (int i = 0; i < mMaps.size(); ++i)
	//{
	//	for (int j = 0; j < mMaps[i].size(); ++j)
	//	{
	//		mMaps[i][j] = 0;
	//	}
	//}
	// 
	// 가운데, 상, 하, 좌, 우, 좌상 대각선, 우상 대각선, 좌하 대각선, 우하 대각선
	mSectorDir =
	{
		{0, 0},
		{ 0, SECTOR_HEIGHT },
		{ 0,  -SECTOR_HEIGHT },
		{ -SECTOR_WIDTH, 0 },
		{ SECTOR_WIDTH, 0 },
		{ -SECTOR_WIDTH, SECTOR_HEIGHT },
		{ SECTOR_WIDTH, SECTOR_HEIGHT },
		{ -SECTOR_WIDTH, -SECTOR_HEIGHT },
		{ SECTOR_WIDTH, -SECTOR_HEIGHT }
	};
}

Channel::~Channel()
{
	for (int i = 0; i < SECTOR_WIDTH_SIZE; ++i)
	{
		delete[] mSectors[i];
	}

	delete[] mSectors;
}

bool Channel::Initialize(int channel)
{
	mSectors = new Sector*[SECTOR_WIDTH_SIZE];

	int objectId = MONSTER_START_ID;
	int x = 0;
	int y = 0;

	int start = MONSTER_START_ID;
	int end = MONSTER_START_ID + SECTOR_MAX_MONSTER;

	for (int i = 0; i < SECTOR_WIDTH_SIZE; ++i)
	{
		mSectors[i] = new Sector[SECTOR_HEIGHT_SIZE];
		for (int j = 0; j < SECTOR_HEIGHT_SIZE; ++j)
		{
			mSectors[i][j].Initialize(channel, start, std::make_pair(i, j), x, y);
			mSectors[i][j].SetObjectStartEndId(start, end);

			y += SECTOR_HEIGHT;

			start = end;
			end += SECTOR_MAX_MONSTER;
		}

		x += SECTOR_WIDTH;
		y = 0;
	}

	return true;
}

int Channel::GetChannelUserSize()
{
	mChannelMtx.lock_shared();
	int size = mUserIdList.size();
	mChannelMtx.unlock_shared();

	return size;
}

bool Channel::IsFull()
{
	mChannelMtx.lock_shared();
	int size = mUserIdList.size();
	mChannelMtx.unlock_shared();

	if (size >= MAX_CHANNEL_USER)
	{
		return true;
	}

	return false;
}

int Channel::PushUser(int id)
{
	int index = -1;
	for (int i = 0; i < MAX_CHANNEL_USER; ++i)
	{
		if (mUserIdList.count(i) == false)
		{
			index = i;
			break;
		}
	}

	tbb::concurrent_hash_map<int, int>::accessor acc;
	mUserIdList.insert(acc, index);
	acc->second = id;

	return index;
}

int Channel::FindUser(int index)	const
{
	tbb::concurrent_hash_map<int, int>::const_accessor acc;
	if (mUserIdList.find(acc, index))
	{
		return acc->second;
	}

	return -1;
}

void Channel::PushSectorObject(int x, int y, int id)
{
	int xNum = x / SECTOR_WIDTH;
	int yNum = y / SECTOR_HEIGHT;

	tbb::concurrent_hash_map<int, int>::accessor acc;
	mSectors[xNum][yNum].GetObjectIds().insert(acc, id);
	acc->second = id;
	acc.release();
}

void Channel::PopSectorObject(int x, int y, int id)
{
	int xNum = x / SECTOR_WIDTH;
	int yNum = y / SECTOR_HEIGHT;
	mSectors[xNum][yNum].GetObjectIds().erase(id);
}

int Channel::FindSectorObjectIndex(int x, int y, int id)
{
	int xNum = x / SECTOR_WIDTH;
	int yNum = y / SECTOR_HEIGHT;

	tbb::concurrent_hash_map<int, int>::const_accessor acc;
	if (mSectors[xNum][yNum].GetObjectIds().find(acc, id))
	{
		return acc->second;
	}
	return -1;
}

int Channel::FindSectorObjectIndexById(int sectorXId, int sectorYId, int id)
{
	tbb::concurrent_hash_map<int, int>::const_accessor acc;
	if (mSectors[sectorXId][sectorYId].GetObjectIds().find(acc, id))
	{
		return acc->second;
	}

	return -1;
}

std::vector<int> Channel::GetUserIds()
{
	std::vector<int> userIds;
	for (int i = 0; i < MAX_CHANNEL_USER; ++i)
	{
		int userId = FindUser(i);
		if (userId == -1)
		{
			continue;
		}

		userIds.emplace_back(userId);
	}

	return userIds;
}

Sector& Channel::FindSector(int x, int y)
{
	int xNum = x / SECTOR_WIDTH;
	int yNum = y / SECTOR_HEIGHT;

	return mSectors[xNum][yNum];
}

bool Channel::CheckSectorRange(int x, int y)
{
	int xNum = x / SECTOR_WIDTH;
	int yNum = y / SECTOR_HEIGHT;

	if (xNum < 0 || xNum >= SECTOR_WIDTH_SIZE || yNum < 0 || yNum>= SECTOR_HEIGHT_SIZE)
	{
		return false;
	}

	return true;
}

std::vector<int> Channel::GetSectorUserIds(int x, int y, bool isPlayer)
{
	std::vector<int> sectorUserIds;
	sectorUserIds.reserve(MAX_CHANNEL_USER);
	
	std::vector<std::pair<int, int>> dirs;
	dirs.reserve(mSectorDir.size());

	if (isPlayer == true)
	{
		dirs = mSectorDir;
	}
	else
	{
		dirs.emplace_back(mSectorDir.front());
	}

	for (int i = 0; i < dirs.size(); ++i)
	{
		int newX = x + mSectorDir[i].first;
		int newY = y + mSectorDir[i].second;

		if (CheckSectorRange(newX, newY) == false)
		{
			continue;
		}

		Sector& sector = FindSector(newX, newY);
		std::vector<int> channelUserIds = GetUserIds();
		for (auto& id : channelUserIds)
		{
			// 범위 섹터 내의 유저만 검색
			if (sector.GetObjectIds().count(id) == false)
			{
				continue;
			}
			
			sectorUserIds.emplace_back(id);
		}
	}

	return sectorUserIds;
}

bool Channel::CompareSector(int x1, int y1, int x2, int y2)
{
	int xNum1 = x1 / SECTOR_WIDTH;
	int xNum2 = x2 / SECTOR_WIDTH;

	int yNum1 = y1 / SECTOR_HEIGHT;
	int yNum2 = y2 / SECTOR_HEIGHT;

	if (xNum1 != xNum2 || yNum1 != yNum2)
	{
		return false;
	}

	return true;
}
