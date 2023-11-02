#include "Channel.h"
#include "../Character/Character.h"

Channel::Channel()
{
	mMonsters = nullptr;
}

Channel::~Channel()
{
	delete[] mMonsters;
}

bool Channel::Initialize(int channel)
{
	int objectId = MONSTER_START_ID;
	// 몬스터는 미리 넣어놓는다.
	mMonsters = new Monster[MAX_MONSTER];
	for (int i = 0; i < MAX_MONSTER; ++i)
	{
		if (mMonsters[i].Inititalize(objectId) == false)
		{
			return false;
		}
		
		mMonsters[i].SetChannel(channel);

		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			mObjectIds.insert(acc, objectId++);
			acc->second = i;
			acc.release();
		}
	}

	return true;
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

int Channel::GetObjectIndex(int id) const
{
	tbb::concurrent_hash_map<int, int>::const_accessor acc;
	if (mObjectIds.find(acc, id))
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
		userIds.emplace_back(userId);
	}

	return userIds;
}
