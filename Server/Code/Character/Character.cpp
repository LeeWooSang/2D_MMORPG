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
	// ���̵�� ����x
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

	// ���� ��ó�� �ִ� ������Ʈ ���̵� ���ο� �丮��Ʈ�� ����
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

	// ���� ������ ��ó�� �ִ� ������Ʈ�鿡 ����
	for (auto id : newViewList)
	{
		int index = GET_INSTANCE(Core)->GetObjectIndex(id);
		// ������ ���� ������Ʈ���
		if (mViewList.count(id) == false)
		{	
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				mViewList.insert(acc, id);
				acc->second = index;
				GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);
			}

			// ������Ʈ�� ���Ͷ�� �����
			if (id >= MONSTER_START_ID)
			{
				//WakeUp(index);
				continue;
			}

			// ���� ������ �丮��Ʈ �˻�
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

		// �������� �ְ�, ���ݵ� ������
		else
		{
			if (id >= MONSTER_START_ID)
			{
				//WakeUp(index);
				continue;
			}

			// ���� ������ �丮��Ʈ �˻�
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

	// �þ߿��� �����
	for (auto obj : oldViewList)
	{
		int id = obj.first;
		int index = obj.second;
		if (newViewList.count(id) == true)
		{
			continue;
		}

		// �� �丮��Ʈ���� ���� ����
		mViewList.erase(id);
		GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, id);

		if (id >= MONSTER_START_ID)
		{
			continue;
		}

		// ���� �丮��Ʈ���� ���� �����.
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
	// ���̵�� ����x

	memset(over->messageBuffer, 0, sizeof(MAX_BUFFER));
	over->dataBuffer.buf = over->messageBuffer;
	over->dataBuffer.len = MAX_BUFFER;

	mPrevSize = 0;
	mSendBytes = 0;
	memset(mPacketBuf, 0, sizeof(mPacketBuf));
	mConnect = false;
	mViewList.clear();

	// ��ǥ ����
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
