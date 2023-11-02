#pragma once
#include "../Character/Character.h"
#include <tbb/tbb.h>
#include <shared_mutex>
#include <vector>

constexpr int MAX_CHANNEL_USER = 200;

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

	int GetObjectIndex(int id) const;
	std::vector<int> GetUserIds();

	Monster* GetMonsters() { return mMonsters; }
	Monster& GetMonster(int index) { return mMonsters[index]; }

private:
	std::shared_mutex mChannelMtx;
	// ¿Œµ¶Ω∫, ¿Ø¿˙ id
	tbb::concurrent_hash_map<int, int> mUserIdList;
	Monster* mMonsters;
	// id, ¿Œµ¶Ω∫
	tbb::concurrent_hash_map<int, int> mObjectIds;
};