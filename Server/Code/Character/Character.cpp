#include "Character.h"
#include "../Core/Core.h"

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
	//mPosMtx.lock();
	int x = mX;
	int y = mY;
	//mPosMtx.unlock();

	switch (dir)
	{
		case DIRECTION_TYPE::UP:
		{
			++y;
			break;
		}
		case DIRECTION_TYPE::DOWN:
		{
			--y;
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

	CheckViewList();
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

void Character::CheckViewList()
{
	int myId = mOver->myId;
	int myIndex = GET_INSTANCE(Core)->GetObjectIndex(myId);
	std::unordered_map<int, int> oldViewList;
	//mViewListMtx.lock_shared();
	mViewListMtx.lock();
	for (auto obj : mViewList)
	{
		oldViewList.emplace(obj.first, obj.second);
	}
	//mViewListMtx.unlock_shared();
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
			int index = GET_INSTANCE(Core)->GetObjectIndex(i);
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
				GET_INSTANCE(Core)->SendPositionPacket(index, myIndex);
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
				GET_INSTANCE(Core)->SendPositionPacket(index, myIndex);
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
