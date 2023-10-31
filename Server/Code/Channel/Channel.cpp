#include "Channel.h"

constexpr int MAX_CHANNEL_USER = 200;

Channel::Channel()
{
}

Channel::~Channel()
{
}

bool Channel::Initialize()
{
	// ���ʹ� �̸� �־���´�.

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

void Channel::PushUser(int id)
{
	tbb::concurrent_hash_map<int, int>::accessor acc;
	mUserIdList.insert(acc, id);
	acc->second = id;
}
