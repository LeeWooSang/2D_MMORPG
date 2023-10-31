#pragma once
//#include "../Common/Protocol.h"
#include <tbb/tbb.h>
#include <shared_mutex>

class Channel
{
public:
	Channel();
	~Channel();
	bool Initialize();
	bool IsFull();
	void PushUser(int id);
	void PopUser(int id) { mUserIdList.erase(id); }

private:
	// Ã¤³Î, id
	tbb::concurrent_hash_map<int, int> mUserIdList;
	std::shared_mutex mChannelMtx;
};