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

	// ���̵�� ����x
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
		int otherIndex = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(id);
		otherX = GET_INSTANCE(Core)->GetChannel(mChannel).GetMonster(otherIndex).GetX();
		otherY = GET_INSTANCE(Core)->GetChannel(mChannel).GetMonster(otherIndex).GetY();

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
	// �丮��Ʈ ����
	for (int i = 0; i < channelUserIds.size(); ++i)
	{
		int id = channelUserIds[i];
		if (id == myId)
		{
			continue;
		}

		// ������ �丮��Ʈ���� ���� ����
		if (users[id].GetViewList().count(myId) == true)
		{
			users[id].GetViewList().erase(myId);
			GET_INSTANCE(Core)->SendRemoveObjectPacket(id, myId);
		}
	}

	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		int index = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(i);
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
	Monster* monsters = GET_INSTANCE(Core)->GetChannel(mChannel).GetMonsters();

	std::vector<int> channelUserIds = GET_INSTANCE(Core)->GetChannel(mChannel).GetUserIds();
	
	// ���� ä�� ������Ʈ�� �˻�
	// ���ӵ� �ٸ� �������Ե� �� ������ ����		
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
		
		// ���� ������ ��, ���̴� �ֵ鿡�Ը� ����(�� �丮��Ʈ�� ���� �����ȵ�)
		// lock, unlock�� �ʹ� �����ϸ� �ȵ�
		// �׷��� ���� ������ lock, unlock��  ���� �ʹ� ��� ������, lock, unlock�� ó�����ϴ� �κи� ���� ó��
		if (CheckDistance(id) == false)
		{
			continue;
		}

		// ������ �ٸ��ֵ� ������ ����
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			mViewList.insert(acc, id);
			acc->second = id;
			GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);
		}
		// �ٸ��ֵ鿡�� ���� ������ ����
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			users[id].GetViewList().insert(acc, myId);
			acc->second = myIndex;
			GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
		}
	}

	// ������ ���̴� ���� ������ ����
	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		if (CheckDistance(i) == false)
		{
			continue;
		}

		int index = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(i);
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
	Monster* monsters = GET_INSTANCE(Core)->GetChannel(mChannel).GetMonsters();

	// ���� ��ó�� �ִ� ������Ʈ ���̵� ���ο� �丮��Ʈ�� ����
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
			// ���� ���� �����߾��µ�, ������ ���������ߴ��ְ� �����ִٸ�? ���������� �����.
			if (mViewList.count(id) == true)
			{
				mViewList.erase(id);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, id);
			}
			continue;
		}

		if (CheckDistance(id) == false)
		{
			// ���� ���� �þ߿� �־��µ�, ������ ���ٸ�? �����׵� �����, �����׵� �����ߴ�
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
			// ���� ���� �þ߿� �־��µ�, ������ ���ٸ�? �����׵� �����, �����׵� �����ߴ�
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
	//			// ���� ���� �����߾��µ�, ������ ���������ߴ��ְ� �����ִٸ�? ���������� �����.
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
	//		// ���� ���� �þ߿� �־��µ�, ������ ���ٸ�? �����׵� �����, �����׵� �����ߴ�
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

	// ���� ������ ��ó�� �ִ� ������Ʈ�鿡 ����
	for (auto id : newViewList)
	{
		// ������ ���� ������Ʈ���
		if (mViewList.count(id) == false)
		{			
			tbb::concurrent_hash_map<int, int>::accessor acc;
			mViewList.insert(acc, id);
			// ���Ͷ��
			if (id >= MONSTER_START_ID)
			{
				int index = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(id);
				acc->second = index;
				acc.release();

				// ���Ͷ�� �����.
				monsters[index].WakeUp();
				GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);
				continue;
			}
			else
			{
				acc->second = id;
				acc.release();
				GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);

				// ���� ������ �丮��Ʈ �˻�
				if (users[id].GetViewList().count(myId) == false)
				{
					tbb::concurrent_hash_map<int, int>::accessor acc2;
					users[id].GetViewList().insert(acc2, myId);
					acc2->second = myIndex;
					acc2.release();
					GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
				}
				else
				{
					GET_INSTANCE(Core)->SendPositionPacket(id, myId);
				}
			}
		}
		// �������� �ְ�, ���ݵ� ������
		else
		{
			if (id >= MONSTER_START_ID)
			{
				int index = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(id);
				monsters[index].WakeUp();
				continue;
			}

			// ���� ������ �丮��Ʈ �˻�
			if (users[id].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[id].GetViewList().insert(acc, myId);
				acc->second = myIndex;
				GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
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
	//	// ������ ���� ������Ʈ���
	//	if (mViewList.count(id) == false)
	//	{
	//		{
	//			tbb::concurrent_hash_map<int, int>::accessor acc;
	//			mViewList.insert(acc, id);
	//			acc->second = index;
	//			GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);
	//		}

	//		// ������Ʈ�� ���Ͷ�� �����
	//		if (id >= MONSTER_START_ID)
	//		{
	//			monsters[index].WakeUp();
	//			continue;
	//		}

	//		// ���� ������ �丮��Ʈ �˻�
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

	//	// �������� �ְ�, ���ݵ� ������
	//	else
	//	{
	//		if (id >= MONSTER_START_ID)
	//		{
	//			monsters[index].WakeUp();
	//			continue;
	//		}

	//		// ���� ������ �丮��Ʈ �˻�
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

void Player::ProcessChangeChannelViewList(int oldChannel, int newChannel)
{
	int myId = mOver->myId;

	Player* users = GET_INSTANCE(Core)->GetUsers();

	// ���� ä�� ������Ʈ���� �丮��Ʈ�� ����
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
			// ���� ���� �����߾��µ�, ������ ���������ߴ��ְ� �����ִٸ�? ���������� �����.
			if (mViewList.count(id) == true)
			{
				mViewList.erase(id);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, id);
			}
			continue;
		}

		// ���濡�� ���� �־��ٸ�
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
		// ���濡�� ���� �־��ٸ�
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

	// ���ο� ä�� ������Ʈ���� �丮��Ʈ �߰�
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

		// ������ �ٸ��ֵ� ������ ����
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			mViewList.insert(acc, id);
			acc->second = id;
			GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);
		}
		// �ٸ��ֵ鿡�� ���� ������ ����
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			users[id].GetViewList().insert(acc, myId);
			acc->second = myId;
			GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
		}
	}

	Monster* newMonsters = GET_INSTANCE(Core)->GetChannel(newChannel).GetMonsters();
	// ������ ���̴� ���� ������ ����
	for (int i = MONSTER_START_ID; i < MAX_OBJECT; ++i)
	{
		if (CheckDistance(i) == false)
		{
			continue;
		}

		int index = GET_INSTANCE(Core)->GetChannel(mChannel).GetObjectIndex(i);
		// ���̴� NPC�� �����.
		newMonsters[index].WakeUp();

		tbb::concurrent_hash_map<int, int>::accessor acc;
		mViewList.insert(acc, i);
		acc->second = index;
		GET_INSTANCE(Core)->SendAddObjectPacket(myId, i);
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
	// Ÿ�̸ӿ� 1�ʵڿ� move ����
	std::chrono::seconds sec{ 1 };
	GET_INSTANCE(GameTimer)->AddTimer(std::chrono::high_resolution_clock::now() + 1s, SERVER_EVENT::MONSTER_MOVE, myId);
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
	// ���� ��ó�� �ִ� ������Ʈ ���̵� ���ο� �丮��Ʈ�� ����
	for (int i = 0; i < channelUserIds.size(); ++i)
	{
		int id = channelUserIds[i];
		if (users[id].GetIsConnect() == false)
		{
			// ���� ���� �����߾��µ�, ������ ���������ߴ��ְ� �����ִٸ�? ���������� �����.
			if (mViewList.count(id) == true)
			{
				mViewList.erase(id);
			}
			continue;
		}

		if (CheckDistance(id) == false)
		{
			// ���� ���� �þ߿� �־��µ�, ������ ���ٸ�? �����׵� �����, �����׵� �����ߴ�
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

	// ���� ������ ��ó�� �ִ� ������Ʈ�鿡 ����
	for (auto id : newViewList)
	{
		// ������ ���� ������Ʈ���
		if (mViewList.count(id) == false)
		{
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				mViewList.insert(acc, id);
				acc->second = id;
			}

			// ���� ������ �丮��Ʈ �˻�
			if (users[id].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[id].GetViewList().insert(acc, myId);
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
			if (users[id].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[id].GetViewList().insert(acc, myId);
				acc->second = myIndex;
				GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
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
