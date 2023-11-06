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

bool Channel::IsFull()
{
	volatile bool result = false;

	mChannelMtx.lock_shared();
	int size = mUserIdList.size();
	mChannelMtx.unlock_shared();

	if (size >= MAX_CHANNEL_USER)
	{
		result = true;
	}

	std::cout << "channel result : " << result << std::endl;
	return result;
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

