#include "Character.h"
#include "../Core/Core.h"
#include "../GameTimer/GameTimer.h"
#include "../Channel/Channel.h"
#include <vector>

Character::Character()
	: mOver(nullptr), mX(0), mY(0)
{
	mChannelIndex = -1;
	mChannel = -1;
}

Character::~Character()
{
	delete mOver;
	mOver = nullptr;
}

void Character::Reset()
{
	memset(&mOver->overlapped, 0, sizeof(WSAOVERLAPPED));
	mOver->eventType = SERVER_EVENT::DEFAULT;
	
	mChannelIndex = -1;
	mChannel = -1;

	// 아이디는 리셋x
}

bool Character::Inititalize(int id)
{
	mOver = new Over;
	mOver->eventType = SERVER_EVENT::DEFAULT;
	mOver->myId = id;

	mChannelIndex = -1;
	mChannel = -1;

	return true;
}

void Character::ProcessMove(char dir)
{
	int x = mX;
	int y = mY;

	switch (dir)
	{
		case DIRECTION_TYPE::UP:
		{
			--y;
			break;
		}
		case DIRECTION_TYPE::DOWN:
		{
			++y;
			break;
		}
		case DIRECTION_TYPE::LEFT:
		{
			--x;
			break;
		}
		case DIRECTION_TYPE::RIGHT:
		{
			++x;
			break;
		}
		default:
		{
			break;
		}
	}

	if (CheckRange(x, y) == false)
	{
		return;
	}

	mX = x;
	mY = y;
}

bool Character::CheckRange(int x, int y)
{
	if (x < 0 || x > WIDTH - 1 || y < 0 || y > HEIGHT - 1)
	{
		return false;
	}

	return true;
}

bool Character::CheckDistance(int id)
{
	int myX = mX;
	int myY = mY;

	int otherX = 0;
	int otherY = 0;
	if (id < MONSTER_START_ID)
	{
		Player* users = GET_INSTANCE(Core)->GetUsers();
		otherX = users[id].GetX();
		otherY = users[id].GetY();
	}
	else
	{
		int index = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(id);
		otherX = GET_INSTANCE(Core)->GetChannel(mChannel).GetMonster(index).GetX();
		otherY = GET_INSTANCE(Core)->GetChannel(mChannel).GetMonster(index).GetY();

	}

	if (VIEW_DISTANCE < std::abs(myX - otherX))
		return false;

	if (VIEW_DISTANCE < std::abs(myY - otherY))
		return false;

	return true;
}

Player::Player()
	: Character(), mSocket(0), mPrevSize(0), mSendBytes(0), mConnect(false)
{
	memset(mPacketBuf, 0, sizeof(mPacketBuf));
}

Player::~Player()
{
	Player::Reset();
}

void Player::Reset()
{
	OverEx* over = static_cast<OverEx*>(mOver);
	memset(&over->overlapped, 0, sizeof(WSAOVERLAPPED));
	over->eventType = SERVER_EVENT::RECV;
	// 아이디는 리셋x

	memset(over->messageBuffer, 0, sizeof(MAX_BUFFER));
	over->dataBuffer.buf = over->messageBuffer;
	over->dataBuffer.len = MAX_BUFFER;

	mPrevSize = 0;
	mSendBytes = 0;
	memset(mPacketBuf, 0, sizeof(mPacketBuf));
	mConnect = false;
	mViewList.clear();

	// 좌표 리셋
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uidX(0, WIDTH - 1);
	std::uniform_int_distribution<int> uidY(0, HEIGHT - 1);
	mX = uidX(dre);
	mY = uidY(dre);

	mChannelIndex = -1;
	mChannel = -1;

	closesocket(mSocket);
}

bool Player::Inititalize(int id)
{
	OverEx* over = new OverEx;
	memset(&over->overlapped, 0, sizeof(WSAOVERLAPPED));	
	over->eventType = SERVER_EVENT::RECV;
	over->myId = id;

	memset(over->messageBuffer, 0, sizeof(MAX_BUFFER));
	over->dataBuffer.buf = over->messageBuffer;
	over->dataBuffer.len = MAX_BUFFER;

	mOver = over;

	mPrevSize = 0;
	mSendBytes = 0;
	memset(mPacketBuf, 0, sizeof(mPacketBuf));
	mConnect = false;
	mSocket = 0;

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uidX(0, WIDTH - 1);
	std::uniform_int_distribution<int> uidY(0, HEIGHT - 1);

	mX = uidX(dre);
	mY = uidY(dre);

	mChannelIndex = -1;
	mChannel = -1;

	return true;
}

void Player::PlayerDisconnect()
{
	int myId = mOver->myId;
	Player* users = GET_INSTANCE(Core)->GetUsers();
	Monster* monsters = GET_INSTANCE(Core)->GetChannel(mChannel).GetMonsters();

	std::vector<int> channelUserIds = GET_INSTANCE(Core)->GetChannel(mChannel).GetUserIds();	
	// 뷰리스트 정리
	for (int i = 0; i < channelUserIds.size(); ++i)
	{
		int id = channelUserIds[i];
		if (id == myId)
		{
			continue;
		}

		// 상대방의 뷰리스트에서 나를 제거
		if (users[id].GetViewList().count(myId) == true)
		{
			users[id].GetViewList().erase(myId);
			GET_INSTANCE(Core)->SendRemoveObjectPacket(id, myId);
		}
	}

	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		int index = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(i);
		// 상대방의 뷰리스트에서 나를 제거
		if (monsters[index].GetViewList().count(myId) == true)
		{
			monsters[index].GetViewList().erase(myId);
		}
	}

	Reset();
}

void Player::ProcessLoginViewList()
{
	int myId = mOver->myId;

	// 내가 누구인지 보내주어야함
	GET_INSTANCE(Core)->SendAddObjectPacket(myId, mChannel, myId);

	Player* users = GET_INSTANCE(Core)->GetUsers();
	Monster* monsters = GET_INSTANCE(Core)->GetChannel(mChannel).GetMonsters();

	std::vector<int> channelUserIds = GET_INSTANCE(Core)->GetChannel(mChannel).GetUserIds();
	
	// 같은 채널 오브젝트만 검색
	// 접속된 다른 유저에게도 내 정보를 보냄		
	for (int i = 0; i < channelUserIds.size(); ++i)
	{
		int id = channelUserIds[i];
		if (id == myId)
		{
			continue;
		}

		if (users[id].GetIsConnect() == false)
		{
			continue;
		}
		
		// 내가 들어왔을 때, 보이는 애들에게만 보냄(내 뷰리스트에 나를 넣음안됨)
		// lock, unlock을 너무 자주하면 안됨
		// 그러나 루프 밖으로 lock, unlock을  빼면 너무 길기 때문에, lock, unlock을 처리해하는 부분만 따로 처리
		if (CheckDistance(id) == false)
		{
			continue;
		}

		// 나에게 다른애들 정보를 보냄
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			mViewList.insert(acc, id);
			acc->second = id;
			GET_INSTANCE(Core)->SendAddObjectPacket(myId, mChannel, id);
		}
		// 다른애들에게 나의 정보를 보냄
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			users[id].GetViewList().insert(acc, myId);
			acc->second = myId;
			GET_INSTANCE(Core)->SendAddObjectPacket(id, mChannel, myId);
		}
	}

	// 나에게 보이는 몬스터 정보를 보냄
	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		if (CheckDistance(i) == false)
		{
			continue;
		}

		int index = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(i);
		// 보이는 NPC를 깨운다.
		monsters[index].WakeUp();

		tbb::concurrent_hash_map<int, int>::accessor acc;
		mViewList.insert(acc, i);
		acc->second = index;
		GET_INSTANCE(Core)->SendAddObjectPacket(myId, mChannel, i);
	}
}

void Player::CheckViewList()
{
	int myId = mOver->myId;
	std::unordered_set<int> newViewList;

	Player* users = GET_INSTANCE(Core)->GetUsers();
	Monster* monsters = GET_INSTANCE(Core)->GetChannel(mChannel).GetMonsters();

	// 나와 근처에 있는 오브젝트 아이디를 새로운 뷰리스트에 넣음
	std::vector<int> channelUserIds = GET_INSTANCE(Core)->GetChannel(mChannel).GetUserIds();
	for (int i = 0; i < channelUserIds.size(); ++i)
	{
		int id = channelUserIds[i];
		if (id == myId)
		{
			continue;
		}

		if (users[id].GetIsConnect() == false)
		{
			// 만약 전에 접속했었는데, 지금은 접속종료했던애가 남아있다면? 내꺼에서만 지운다.
			if (mViewList.count(id) == true)
			{
				mViewList.erase(id);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, id);
			}
			continue;
		}

		if (CheckDistance(id) == false)
		{
			// 만약 전에 시야에 있었는데, 지금은 없다면? 나한테도 지우고, 걔한테도 지워야댐
			if (mViewList.count(id) == true)
			{
				mViewList.erase(id);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, id);
			}

			if (users[id].GetViewList().count(myId) == true)
			{
				users[id].GetViewList().erase(myId);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(id, myId);
			}			
			continue;
		}
		newViewList.emplace(id);
	}
	
	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		if (CheckDistance(i) == false)
		{
			// 만약 전에 시야에 있었는데, 지금은 없다면? 나한테도 지우고, 걔한테도 지워야댐
			if (mViewList.count(i) == true)
			{
				mViewList.erase(i);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, i);
			}

			int index = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(i);
			if (monsters[index].GetViewList().count(myId) == true)
			{
				monsters[index].GetViewList().erase(myId);
			}

			continue;
		}
		newViewList.emplace(i);
	}

	//////////////////////////////////////////////////////////
	//for (int i = 0; i < MAX_OBJECT; ++i)
	//{
	//	if (myId == i)
	//	{
	//		continue;
	//	}

	//	int index = GET_INSTANCE(Core)->GetObjectIndex(i);
	//	if (i < MONSTER_START_ID)
	//	{
	//		if (users[index].GetIsConnect() == false)
	//		{
	//			// 만약 전에 접속했었는데, 지금은 접속종료했던애가 남아있다면? 내꺼에서만 지운다.
	//			if (mViewList.count(i) == true)
	//			{
	//				mViewList.erase(i);
	//				GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, i);
	//			}
	//			continue;
	//		}
	//	}

	//	if (CheckDistance(i) == false)
	//	{
	//		// 만약 전에 시야에 있었는데, 지금은 없다면? 나한테도 지우고, 걔한테도 지워야댐
	//		if (mViewList.count(i) == true)
	//		{
	//			mViewList.erase(i);
	//			GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, i);
	//		}

	//		if (i < MONSTER_START_ID)
	//		{
	//			if (users[index].GetViewList().count(myId) == true)
	//			{
	//				users[index].GetViewList().erase(myId);
	//				GET_INSTANCE(Core)->SendRemoveObjectPacket(i, myId);
	//			}
	//		}
	//		else
	//		{
	//			if (monsters[index].GetViewList().count(myId) == true)
	//			{
	//				monsters[index].GetViewList().erase(myId);
	//			}
	//		}
	//		continue;
	//	}
	//	newViewList.emplace(i);
	//}

	// 나와 이전에 근처에 있던 오브젝트들에 대해
	for (auto id : newViewList)
	{
		// 이전에 없던 오브젝트라면
		if (mViewList.count(id) == false)
		{			
			tbb::concurrent_hash_map<int, int>::accessor acc;
			mViewList.insert(acc, id);
			// 몬스터라면
			if (id >= MONSTER_START_ID)
			{
				int index = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(id);
				acc->second = index;
				acc.release();

				// 몬스터라면 깨운다.
				monsters[index].WakeUp();
				GET_INSTANCE(Core)->SendAddObjectPacket(myId, mChannel, id);
				continue;
			}
			else
			{
				acc->second = id;
				acc.release();
				GET_INSTANCE(Core)->SendAddObjectPacket(myId, mChannel, id);

				// 상대방 유저의 뷰리스트 검사
				if (users[id].GetViewList().count(myId) == false)
				{
					tbb::concurrent_hash_map<int, int>::accessor acc2;
					users[id].GetViewList().insert(acc2, myId);
					acc2->second = myId;
					acc2.release();
					GET_INSTANCE(Core)->SendAddObjectPacket(id, mChannel, myId);
				}
				else
				{
					GET_INSTANCE(Core)->SendPositionPacket(id, myId);
				}
			}
		}
		// 이전에도 있고, 지금도 존재함
		else
		{
			if (id >= MONSTER_START_ID)
			{
				int index = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(id);
				monsters[index].WakeUp();
				continue;
			}

			// 상대방 유저의 뷰리스트 검사
			if (users[id].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[id].GetViewList().insert(acc, myId);
				acc->second = myId;
				GET_INSTANCE(Core)->SendAddObjectPacket(id, mChannel, myId);
			}
			else
			{
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
		}
	}

	//for (auto id : newViewList)
	//{
	//	int index = GET_INSTANCE(Core)->GetObjectIndex(id);
	//	// 이전에 없던 오브젝트라면
	//	if (mViewList.count(id) == false)
	//	{
	//		{
	//			tbb::concurrent_hash_map<int, int>::accessor acc;
	//			mViewList.insert(acc, id);
	//			acc->second = index;
	//			GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);
	//		}

	//		// 오브젝트가 몬스터라면 깨운다
	//		if (id >= MONSTER_START_ID)
	//		{
	//			monsters[index].WakeUp();
	//			continue;
	//		}

	//		// 상대방 유저의 뷰리스트 검사
	//		if (users[index].GetViewList().count(myId) == false)
	//		{
	//			tbb::concurrent_hash_map<int, int>::accessor acc;
	//			users[index].GetViewList().insert(acc, myId);
	//			acc->second = myIndex;
	//			GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
	//		}
	//		else
	//		{
	//			GET_INSTANCE(Core)->SendPositionPacket(id, myId);
	//		}
	//	}

	//	// 이전에도 있고, 지금도 존재함
	//	else
	//	{
	//		if (id >= MONSTER_START_ID)
	//		{
	//			monsters[index].WakeUp();
	//			continue;
	//		}

	//		// 상대방 유저의 뷰리스트 검사
	//		if (users[index].GetViewList().count(myId) == false)
	//		{
	//			tbb::concurrent_hash_map<int, int>::accessor acc;
	//			users[index].GetViewList().insert(acc, myId);
	//			acc->second = myIndex;
	//			GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
	//		}
	//		else
	//		{
	//			GET_INSTANCE(Core)->SendPositionPacket(id, myId);
	//		}
	//	}
	//}
}

void Player::ProcessChangeChannelViewList(int oldChannel, int newChannel)
{
	int myId = mOver->myId;

	Player* users = GET_INSTANCE(Core)->GetUsers();

	// 이전 채널 오브젝트들의 뷰리스트를 정리
	std::vector<int> oldChannelUserIds = GET_INSTANCE(Core)->GetChannel(oldChannel).GetUserIds();
	for (int i = 0; i < oldChannelUserIds.size(); ++i)
	{
		int id = oldChannelUserIds[i];
		if (id == myId)
		{
			continue;
		}

		if (users[id].GetIsConnect() == false)
		{
			// 만약 전에 접속했었는데, 지금은 접속종료했던애가 남아있다면? 내꺼에서만 지운다.
			if (mViewList.count(id) == true)
			{
				mViewList.erase(id);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, id);
			}
			continue;
		}

		// 상대방에게 내가 있었다면
		if (users[id].GetViewList().count(myId) == true)
		{
			users[id].GetViewList().erase(myId);
			GET_INSTANCE(Core)->SendRemoveObjectPacket(id, myId);
		}

		if (mViewList.count(id) == true)
		{
			mViewList.erase(id);
			GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, id);
		}
	}

	Monster* oldMonsters = GET_INSTANCE(Core)->GetChannel(oldChannel).GetMonsters();
	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		int index = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(i);
		// 상대방에게 내가 있었다면
		if (oldMonsters[index].GetViewList().count(myId) == true)
		{
			oldMonsters[index].GetViewList().erase(myId);
		}

		if (mViewList.count(i) == true)
		{
			mViewList.erase(i);
			GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, i);
		}
	}

	// 새로운 채널 오브젝트들의 뷰리스트 추가
	std::vector<int> newChannelUserIds = GET_INSTANCE(Core)->GetChannel(newChannel).GetUserIds();
	for (int i = 0; i < newChannelUserIds.size(); ++i)
	{
		int id = newChannelUserIds[i];
		if (id == myId)
		{
			continue;
		}

		if (users[id].GetIsConnect() == false)
		{
			continue;
		}

		if (CheckDistance(id) == false)
		{
			continue;
		}

		// 나에게 다른애들 정보를 보냄
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			mViewList.insert(acc, id);
			acc->second = id;
			GET_INSTANCE(Core)->SendAddObjectPacket(myId, mChannel, id);
		}
		// 다른애들에게 나의 정보를 보냄
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			users[id].GetViewList().insert(acc, myId);
			acc->second = myId;
			GET_INSTANCE(Core)->SendAddObjectPacket(id, mChannel, myId);
		}
	}

	Monster* newMonsters = GET_INSTANCE(Core)->GetChannel(newChannel).GetMonsters();
	// 나에게 보이는 몬스터 정보를 보냄
	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		if (CheckDistance(i) == false)
		{
			continue;
		}

		int index = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(i);
		// 보이는 NPC를 깨운다.
		newMonsters[index].WakeUp();

		tbb::concurrent_hash_map<int, int>::accessor acc;
		mViewList.insert(acc, i);
		acc->second = index;
		GET_INSTANCE(Core)->SendAddObjectPacket(myId, mChannel, i);
	}
}

Monster::Monster()
	: Character(), mSleep(true)
{
}

Monster::~Monster()
{
	Reset();
}

void Monster::Reset()
{
	Character::Reset();
	mSleep = true;
}

bool Monster::Inititalize(int id)
{
	mOver = new Over;
	mOver->eventType = SERVER_EVENT::DEFAULT;
	mOver->myId = id;

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uidX(0, WIDTH - 1);
	std::uniform_int_distribution<int> uidY(0, HEIGHT - 1);

	mX = uidX(dre);
	mY = uidY(dre);

	mChannelIndex = -1;
	mChannel = -1;

	mSleep = true;

	return true;
}

using namespace std;
void Monster::WakeUp()
{
	int myId = mOver->myId;
	// 타이머에 1초뒤에 move 전달
	std::chrono::seconds sec{ 1 };
	GET_INSTANCE(GameTimer)->AddTimer(std::chrono::high_resolution_clock::now() + 1s, SERVER_EVENT::MONSTER_MOVE, myId, mChannel);
}

char Monster::RandomDirection() const
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uidDir(DIRECTION_TYPE::UP, DIRECTION_TYPE::RIGHT);

	return static_cast<char>(uidDir(dre));
}

void Monster::ProcessMoveViewList()
{
	int myId = mOver->myId;
	int myIndex = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(myId);
	std::unordered_set<int> newViewList;

	Player* users = GET_INSTANCE(Core)->GetUsers();

	std::vector<int> channelUserIds = GET_INSTANCE(Core)->GetChannel(mChannel).GetUserIds();
	// 나와 근처에 있는 오브젝트 아이디를 새로운 뷰리스트에 넣음
	for (int i = 0; i < channelUserIds.size(); ++i)
	{
		int id = channelUserIds[i];
		if (users[id].GetIsConnect() == false)
		{
			// 만약 전에 접속했었는데, 지금은 접속종료했던애가 남아있다면? 내꺼에서만 지운다.
			if (mViewList.count(id) == true)
			{
				mViewList.erase(id);
			}
			continue;
		}

		if (CheckDistance(id) == false)
		{
			// 만약 전에 시야에 있었는데, 지금은 없다면? 나한테도 지우고, 걔한테도 지워야댐
			if (mViewList.count(id) == true)
			{
				mViewList.erase(id);
			}

			if (users[id].GetViewList().count(myId) == true)
			{
				users[id].GetViewList().erase(myId);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(id, myId);
			}
			
			continue;
		}
		newViewList.emplace(id);
	}

	// 나와 이전에 근처에 있던 오브젝트들에 대해
	for (auto id : newViewList)
	{
		// 이전에 없던 오브젝트라면
		if (mViewList.count(id) == false)
		{
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				mViewList.insert(acc, id);
				acc->second = id;
			}

			// 상대방 유저의 뷰리스트 검사
			if (users[id].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[id].GetViewList().insert(acc, myId);
				acc->second = myIndex;
				GET_INSTANCE(Core)->SendAddObjectPacket(id, mChannel, myId);
			}
			else
			{
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
		}

		// 이전에도 있고, 지금도 존재함
		else
		{
			// 상대방 유저의 뷰리스트 검사
			if (users[id].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[id].GetViewList().insert(acc, myId);
				acc->second = myIndex;
				GET_INSTANCE(Core)->SendAddObjectPacket(id, mChannel, myId);
			}
			else
			{
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
		}
	}

	//if (newViewList.size() > 0)
	//{
	//	WakeUp();
	//}
}
