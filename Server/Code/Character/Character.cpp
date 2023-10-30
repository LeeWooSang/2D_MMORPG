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

void Player::PlayerDisconnect()
{
	int myId = mOver->myId;
	Player* users = GET_INSTANCE(Core)->GetUsers();
	Monster* monsters = GET_INSTANCE(Core)->GetMonsters();

	// �丮��Ʈ ����
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (i == myId)
		{
			continue;
		}

		// ������ �丮��Ʈ���� ���� ����
		if (users[i].GetViewList().count(myId) == true)
		{
			users[i].GetViewList().erase(myId);
			GET_INSTANCE(Core)->SendRemoveObjectPacket(i, myId);
		}
	}

	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		int index = GET_INSTANCE(Core)->GetObjectIndex(i);
		// ������ �丮��Ʈ���� ���� ����
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

	// ���� �������� �����־����
	GET_INSTANCE(Core)->SendAddObjectPacket(myId, myId);

	Player* users = GET_INSTANCE(Core)->GetUsers();
	Monster* monsters = GET_INSTANCE(Core)->GetMonsters();

	// ���ӵ� �ٸ� �������Ե� �� ������ ����
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
		
		// ���� ������ ��, ���̴� �ֵ鿡�Ը� ����(�� �丮��Ʈ�� ���� �����ȵ�)
		// lock, unlock�� �ʹ� �����ϸ� �ȵ�
		// �׷��� ���� ������ lock, unlock��  ���� �ʹ� ��� ������, lock, unlock�� ó�����ϴ� �κи� ���� ó��
		if (CheckDistance(i) == false)
		{
			continue;
		}

		// ������ �ٸ��ֵ� ������ ����
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			mViewList.insert(acc, i);
			acc->second = index;
			GET_INSTANCE(Core)->SendAddObjectPacket(myId, i);
		}
		// �ٸ��ֵ鿡�� ���� ������ ����
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			users[index].GetViewList().insert(acc, myId);
			acc->second = myIndex;
			GET_INSTANCE(Core)->SendAddObjectPacket(i, myId);
		}
	}

	// ������ ���̴� ���� ������ ����
	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		if (CheckDistance(i) == false)
		{
			continue;
		}

		int index = GET_INSTANCE(Core)->GetObjectIndex(i);
		// ���̴� NPC�� �����.
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

	// ���� ��ó�� �ִ� ������Ʈ ���̵� ���ο� �丮��Ʈ�� ����
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
				// ���� ���� �����߾��µ�, ������ ���������ߴ��ְ� �����ִٸ�? ���������� �����.
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
			// ���� ���� �þ߿� �־��µ�, ������ ���ٸ�? �����׵� �����, �����׵� �����ߴ�
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
				monsters[index].WakeUp();
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
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
		}

		// �������� �ְ�, ���ݵ� ������
		else
		{
			if (id >= MONSTER_START_ID)
			{
				monsters[index].WakeUp();
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

	// ���� ��ó�� �ִ� ������Ʈ ���̵� ���ο� �丮��Ʈ�� ����
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
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
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
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
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

	// ���� ��ó�� �ִ� ������Ʈ ���̵� ���ο� �丮��Ʈ�� ����
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

	// ���� ������ ��ó�� �ִ� ������Ʈ�鿡 ����
	for (auto id : newViewList)
	{
		int index = GET_INSTANCE(Core)->GetObjectIndex(id);

		mViewListMtx.lock();
		// ������ ���� ������Ʈ���
		if (mSTLViewList.count(id) == false)
		{
			mSTLViewList.emplace(id);
			mViewListMtx.unlock();
			GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);

			// ������Ʈ�� ���Ͷ�� �����
			if (id >= MONSTER_START_ID)
			{
				//WakeUp(index);
				continue;
			}

			// ���� ������ �丮��Ʈ �˻�
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

		// �������� �ְ�, ���ݵ� ������
		else
		{
			mViewListMtx.unlock();
			if (id >= MONSTER_START_ID)
			{
				//WakeUp(index);
				continue;
			}

			// ���� ������ �丮��Ʈ �˻�
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

	//�þ߿��� �����
	for (auto id : oldViewList)
	{
		if (newViewList.count(id) == true)
		{
			continue;
		}

		mViewListMtx.lock();
		// �� �丮��Ʈ���� ���� ����
		mSTLViewList.erase(id);
		mViewListMtx.unlock();
		GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, id);

		if (id >= MONSTER_START_ID)
		{
			continue;
		}

		int index = GET_INSTANCE(Core)->GetObjectIndex(id);
		// ���� �丮��Ʈ���� ���� �����.
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
	// Ÿ�̸ӿ� 1�ʵڿ� move ����
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
	// ���� ��ó�� �ִ� ������Ʈ ���̵� ���ο� �丮��Ʈ�� ����
	for (int i = 0; i < MAX_USER; ++i)
	{
		int index = GET_INSTANCE(Core)->GetObjectIndex(i);
		if (users[index].GetIsConnect() == false)
		{
			// ���� ���� �����߾��µ�, ������ ���������ߴ��ְ� �����ִٸ�? ���������� �����.
			if (mViewList.count(i) == true)
			{
				mViewList.erase(i);
			}
			continue;
		}

		if (CheckDistance(i) == false)
		{
			// ���� ���� �þ߿� �־��µ�, ������ ���ٸ�? �����׵� �����, �����׵� �����ߴ�
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
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
		}

		// �������� �ְ�, ���ݵ� ������
		else
		{
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
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
		}
	}

	if (newViewList.size() > 0)
	{
		WakeUp();
	}
}
