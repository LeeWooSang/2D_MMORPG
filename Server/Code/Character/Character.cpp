#include "Character.h"
#include "../Core/Core.h"
#include "../GameTimer/GameTimer.h"

Character::Character()
	: mOver(nullptr), mX(0), mY(0)
{
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
	// 아이디는 리셋x
}

bool Character::Inititalize(int id)
{
	mOver = new Over;
	mOver->eventType = SERVER_EVENT::DEFAULT;
	mOver->myId = id;

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
	int index = GET_INSTANCE(Core)->GetObjectIndex(id);
	if (index == -1)
	{
		return false;
	}

	int myX = mX;
	int myY = mY;

	int otherX = 0;
	int otherY = 0;
	if (id < MONSTER_START_ID)
	{
		otherX = GET_INSTANCE(Core)->GetUser(index).GetX();
		otherY = GET_INSTANCE(Core)->GetUser(index).GetY();
	}
	else
	{
		otherX = GET_INSTANCE(Core)->GetMonster(index).GetX();
		otherY = GET_INSTANCE(Core)->GetMonster(index).GetY();
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

	return true;
}

void Player::PlayerDisconnect()
{
	int myId = mOver->myId;
	Player* users = GET_INSTANCE(Core)->GetUsers();
	Monster* monsters = GET_INSTANCE(Core)->GetMonsters();

	// 뷰리스트 정리
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (i == myId)
		{
			continue;
		}

		// 상대방의 뷰리스트에서 나를 제거
		if (users[i].GetViewList().count(myId) == true)
		{
			users[i].GetViewList().erase(myId);
			GET_INSTANCE(Core)->SendRemoveObjectPacket(i, myId);
		}
	}

	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		int index = GET_INSTANCE(Core)->GetObjectIndex(i);
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
	int myIndex = GET_INSTANCE(Core)->GetObjectIndex(myId);

	// 내가 누구인지 보내주어야함
	GET_INSTANCE(Core)->SendAddObjectPacket(myId, myId);

	Player* users = GET_INSTANCE(Core)->GetUsers();
	Monster* monsters = GET_INSTANCE(Core)->GetMonsters();

	// 접속된 다른 유저에게도 내 정보를 보냄
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (i == myId)
		{
			continue;
		}

		int index = GET_INSTANCE(Core)->GetObjectIndex(i);
		if (users[index].GetIsConnect() == false)
		{
			continue;
		}
		
		// 내가 들어왔을 때, 보이는 애들에게만 보냄(내 뷰리스트에 나를 넣음안됨)
		// lock, unlock을 너무 자주하면 안됨
		// 그러나 루프 밖으로 lock, unlock을  빼면 너무 길기 때문에, lock, unlock을 처리해하는 부분만 따로 처리
		if (CheckDistance(i) == false)
		{
			continue;
		}

		// 나에게 다른애들 정보를 보냄
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			mViewList.insert(acc, i);
			acc->second = index;
			GET_INSTANCE(Core)->SendAddObjectPacket(myId, i);
		}
		// 다른애들에게 나의 정보를 보냄
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			users[index].GetViewList().insert(acc, myId);
			acc->second = myIndex;
			GET_INSTANCE(Core)->SendAddObjectPacket(i, myId);
		}
	}

	// 나에게 보이는 몬스터 정보를 보냄
	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		if (CheckDistance(i) == false)
		{
			continue;
		}

		int index = GET_INSTANCE(Core)->GetObjectIndex(i);
		// 보이는 NPC를 깨운다.
		monsters[index].WakeUp();

		tbb::concurrent_hash_map<int, int>::accessor acc;
		mViewList.insert(acc, i);
		acc->second = index;
		GET_INSTANCE(Core)->SendAddObjectPacket(myId, i);
	}
}

void Player::CheckViewList()
{
	int myId = mOver->myId;
	int myIndex = GET_INSTANCE(Core)->GetObjectIndex(myId);
	std::unordered_set<int> newViewList;

	Player* users = GET_INSTANCE(Core)->GetUsers();
	Monster* monsters = GET_INSTANCE(Core)->GetMonsters();

	// 나와 근처에 있는 오브젝트 아이디를 새로운 뷰리스트에 넣음
	for (int i = 0; i < MAX_OBJECT; ++i)
	{
		if (myId == i)
		{
			continue;
		}

		int index = GET_INSTANCE(Core)->GetObjectIndex(i);
		if (i < MONSTER_START_ID)
		{
			if (users[index].GetIsConnect() == false)
			{
				// 만약 전에 접속했었는데, 지금은 접속종료했던애가 남아있다면? 내꺼에서만 지운다.
				if (mViewList.count(i) == true)
				{
					mViewList.erase(i);
					GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, i);
				}
				continue;
			}
		}

		if (CheckDistance(i) == false)
		{
			// 만약 전에 시야에 있었는데, 지금은 없다면? 나한테도 지우고, 걔한테도 지워야댐
			if (mViewList.count(i) == true)
			{
				mViewList.erase(i);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, i);
			}

			if (i < MONSTER_START_ID)
			{
				if (users[index].GetViewList().count(myId) == true)
				{
					users[index].GetViewList().erase(myId);
					GET_INSTANCE(Core)->SendRemoveObjectPacket(i, myId);
				}
			}
			else
			{
				if (monsters[index].GetViewList().count(myId) == true)
				{
					monsters[index].GetViewList().erase(myId);
				}
			}
			continue;
		}
		newViewList.emplace(i);
	}

	// 나와 이전에 근처에 있던 오브젝트들에 대해
	for (auto id : newViewList)
	{
		int index = GET_INSTANCE(Core)->GetObjectIndex(id);
		// 이전에 없던 오브젝트라면
		if (mViewList.count(id) == false)
		{
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				mViewList.insert(acc, id);
				acc->second = index;
				GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);
			}

			// 오브젝트가 몬스터라면 깨운다
			if (id >= MONSTER_START_ID)
			{
				monsters[index].WakeUp();
				continue;
			}

			// 상대방 유저의 뷰리스트 검사
			if (users[index].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[index].GetViewList().insert(acc, myId);
				acc->second = myIndex;
				GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
			}
			else
			{
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
		}

		// 이전에도 있고, 지금도 존재함
		else
		{
			if (id >= MONSTER_START_ID)
			{
				monsters[index].WakeUp();
				continue;
			}

			// 상대방 유저의 뷰리스트 검사
			if (users[index].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[index].GetViewList().insert(acc, myId);
				acc->second = myIndex;
				GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
			}
			else
			{
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
		}
	}
}

void Player::CheckOldViewList()
{
	int myId = mOver->myId;
	int myIndex = GET_INSTANCE(Core)->GetObjectIndex(myId);
	std::unordered_map<int, int> oldViewList;
	mViewListMtx.lock();
	for (auto obj : mViewList)
	{
		oldViewList.emplace(obj.first, obj.second);
	}
	mViewListMtx.unlock();

	std::unordered_set<int> newViewList;

	Player* users = GET_INSTANCE(Core)->GetUsers();
	Monster* monsters = GET_INSTANCE(Core)->GetMonsters();

	// 나와 근처에 있는 오브젝트 아이디를 새로운 뷰리스트에 넣음
	for (int i = 0; i < MAX_OBJECT; ++i)
	{
		if (myId == i)
		{
			continue;
		}

		if (i < MONSTER_START_ID)
		{
			if (users[i].GetIsConnect() == false)
			{
				continue;
			}
		}

		if (CheckDistance(i) == false)
		{
			continue;
		}

		newViewList.emplace(i);
	}

	// 나와 이전에 근처에 있던 오브젝트들에 대해
	for (auto id : newViewList)
	{
		int index = GET_INSTANCE(Core)->GetObjectIndex(id);
		// 이전에 없던 오브젝트라면
		if (mViewList.count(id) == false)
		{
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				mViewList.insert(acc, id);
				acc->second = index;
				GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);
			}

			// 오브젝트가 몬스터라면 깨운다
			if (id >= MONSTER_START_ID)
			{
				//WakeUp(index);
				continue;
			}

			// 상대방 유저의 뷰리스트 검사
			if (users[index].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[index].GetViewList().insert(acc, myId);
				acc->second = myIndex;
				GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
			}
			else
			{
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
		}

		// 이전에도 있고, 지금도 존재함
		else
		{
			if (id >= MONSTER_START_ID)
			{
				//WakeUp(index);
				continue;
			}

			// 상대방 유저의 뷰리스트 검사
			if (users[index].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[index].GetViewList().insert(acc, myId);
				acc->second = myIndex;
				GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
			}
			else
			{
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
		}
	}

	// 시야에서 사라짐
	for (auto obj : oldViewList)
	{
		int id = obj.first;
		int index = obj.second;
		if (newViewList.count(id) == true)
		{
			continue;
		}

		// 내 뷰리스트에서 상대방 제거
		mViewList.erase(id);
		GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, id);

		if (id >= MONSTER_START_ID)
		{
			continue;
		}

		// 상대방 뷰리스트에도 나를 지운다.
		if (users[index].GetViewList().count(myId) == true)
		{
			users[index].GetViewList().erase(myId);
			GET_INSTANCE(Core)->SendRemoveObjectPacket(id, myId);
		}
	}
}

void Player::CheckSTLViewList()
{
	int myId = mOver->myId;
	int myIndex = GET_INSTANCE(Core)->GetObjectIndex(myId);

	mViewListMtx.lock();
	std::unordered_set<int> oldViewList = mSTLViewList;
	mViewListMtx.unlock();
	std::unordered_set<int> newViewList;

	Player* users = GET_INSTANCE(Core)->GetUsers();
	Monster* monsters = GET_INSTANCE(Core)->GetMonsters();

	// 나와 근처에 있는 오브젝트 아이디를 새로운 뷰리스트에 넣음
	for (int i = 0; i < MAX_OBJECT; ++i)
	{
		if (myId == i)
		{
			continue;
		}

		int index = GET_INSTANCE(Core)->GetObjectIndex(i);
		if (i < MONSTER_START_ID)
		{
			if (users[index].GetIsConnect() == false)
			{
				continue;
			}
		}

		if (CheckDistance(i) == false)
		{
			continue;
		}
		newViewList.emplace(i);
	}

	// 나와 이전에 근처에 있던 오브젝트들에 대해
	for (auto id : newViewList)
	{
		int index = GET_INSTANCE(Core)->GetObjectIndex(id);

		mViewListMtx.lock();
		// 이전에 없던 오브젝트라면
		if (mSTLViewList.count(id) == false)
		{
			mSTLViewList.emplace(id);
			mViewListMtx.unlock();
			GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);

			// 오브젝트가 몬스터라면 깨운다
			if (id >= MONSTER_START_ID)
			{
				//WakeUp(index);
				continue;
			}

			// 상대방 유저의 뷰리스트 검사
			users[index].GetViewListMtx().lock();
			if (users[index].GetSTLViewList().count(myId) == false)
			{
				users[index].GetSTLViewList().emplace(myId);
				users[index].GetViewListMtx().unlock();
				GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
			}
			else
			{
				users[index].GetViewListMtx().unlock();
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
		}

		// 이전에도 있고, 지금도 존재함
		else
		{
			mViewListMtx.unlock();
			if (id >= MONSTER_START_ID)
			{
				//WakeUp(index);
				continue;
			}

			// 상대방 유저의 뷰리스트 검사
			users[index].GetViewListMtx().lock();
			if (users[index].GetSTLViewList().count(myId) == false)
			{
				users[index].GetSTLViewList().emplace(myId);
				users[index].GetViewListMtx().unlock();
				GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
			}
			else
			{
				users[index].GetViewListMtx().unlock();
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
		}
	}

	//시야에서 사라짐
	for (auto id : oldViewList)
	{
		if (newViewList.count(id) == true)
		{
			continue;
		}

		mViewListMtx.lock();
		// 내 뷰리스트에서 상대방 제거
		mSTLViewList.erase(id);
		mViewListMtx.unlock();
		GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, id);

		if (id >= MONSTER_START_ID)
		{
			continue;
		}

		int index = GET_INSTANCE(Core)->GetObjectIndex(id);
		// 상대방 뷰리스트에도 나를 지운다.
		users[index].GetViewListMtx().lock();
		if (users[index].GetViewList().count(myId) == true)
		{
			users[index].GetViewList().erase(myId);
			users[index].GetViewListMtx().unlock();
			GET_INSTANCE(Core)->SendRemoveObjectPacket(id, myId);
		}
		else
		{
			users[index].GetViewListMtx().unlock();
		}
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

	mSleep = true;

	return true;
}

using namespace std;
void Monster::WakeUp()
{
	int myId = mOver->myId;
	// 타이머에 1초뒤에 move 전달
	std::chrono::seconds sec{ 1 };
	GET_INSTANCE(GameTimer)->AddTimer(std::chrono::high_resolution_clock::now() + 1s, SERVER_EVENT::MONSTER_MOVE, myId);
}

void Monster::Sleep()
{
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
	int myIndex = GET_INSTANCE(Core)->GetObjectIndex(myId);
	std::unordered_set<int> newViewList;

	Player* users = GET_INSTANCE(Core)->GetUsers();
	// 나와 근처에 있는 오브젝트 아이디를 새로운 뷰리스트에 넣음
	for (int i = 0; i < MAX_USER; ++i)
	{
		int index = GET_INSTANCE(Core)->GetObjectIndex(i);
		if (users[index].GetIsConnect() == false)
		{
			// 만약 전에 접속했었는데, 지금은 접속종료했던애가 남아있다면? 내꺼에서만 지운다.
			if (mViewList.count(i) == true)
			{
				mViewList.erase(i);
			}
			continue;
		}

		if (CheckDistance(i) == false)
		{
			// 만약 전에 시야에 있었는데, 지금은 없다면? 나한테도 지우고, 걔한테도 지워야댐
			if (mViewList.count(i) == true)
			{
				mViewList.erase(i);
			}

			if (users[index].GetViewList().count(myId) == true)
			{
				users[index].GetViewList().erase(myId);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(i, myId);
			}
			
			continue;
		}
		newViewList.emplace(i);
	}

	// 나와 이전에 근처에 있던 오브젝트들에 대해
	for (auto id : newViewList)
	{
		int index = GET_INSTANCE(Core)->GetObjectIndex(id);
		// 이전에 없던 오브젝트라면
		if (mViewList.count(id) == false)
		{
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				mViewList.insert(acc, id);
				acc->second = index;
			}

			// 상대방 유저의 뷰리스트 검사
			if (users[index].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[index].GetViewList().insert(acc, myId);
				acc->second = myIndex;
				GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
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
			if (users[index].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[index].GetViewList().insert(acc, myId);
				acc->second = myIndex;
				GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
			}
			else
			{
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
		}
	}

	if (newViewList.size() > 0)
	{
		WakeUp();
	}
}
