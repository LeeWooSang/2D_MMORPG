#include "Character.h"
#include "../Core/Core.h"
#include "../GameTimer/GameTimer.h"
#include "../Channel/Channel.h"
#include "../Sector/Sector.h"
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
	int oldX = mX;
	int oldY = mY;
	int newX = mX;
	int newY = mY;

	switch (dir)
	{
		case DIRECTION_TYPE::UP:
		{
			--newY;
			break;
		}
		case DIRECTION_TYPE::DOWN:
		{
			++newY;
			break;
		}
		case DIRECTION_TYPE::LEFT:
		{
			--newX;
			break;
		}
		case DIRECTION_TYPE::RIGHT:
		{
			++newX;
			break;
		}
		default:
		{
			break;
		}
	}

	if (CheckRange(newX, newY) == false)
	{
		return;
	}

	ProcessChangeSector(oldX, oldY, newX, newY);

	mX = newX;
	mY = newY;
}

bool Character::CheckRange(int x, int y)
{
	if (x < 0 || x > WIDTH - 1 || y < 0 || y > HEIGHT - 1)
	{
		return false;
	}

	return true;
}

bool Character::CheckDistance(int x, int y)
{
	int myX = mX;
	int myY = mY;

	if (VIEW_DISTANCE < std::abs(myX - x))
	{
		return false;
	}
		
	if (VIEW_DISTANCE < std::abs(myY - y))
	{
		return false;
	}

	return true;
}

void Character::ProcessChangeSector(int oldX, int oldY, int newX, int newY)
{
	// ���Ͱ� �ٲ��� �ʾ����� ����
	if (GET_INSTANCE(Core)->GetChannel(mChannel).CompareSector(oldX, oldY, newX, newY) == true)
	{
		return;
	}

	int myId = mOver->myId;
	Player* users = GET_INSTANCE(Core)->GetUsers();

	std::unordered_map<int, int> newViewList;

	// ���� ���� ����
	GET_INSTANCE(Core)->GetChannel(mChannel).PopSectorObject(oldX, oldY, myId);

	// ���� ������ ���� �丮��Ʈ ����
	std::vector<int> oldSectorUserIds = GET_INSTANCE(Core)->GetChannel(mChannel).GetSectorUserIds(oldX, oldY);
	for (auto& id : oldSectorUserIds)
	{
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

		int x = users[id].GetX();
		int y = users[id].GetY();
		if (CheckDistance(x, y) == false)
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
	}

	// ���� ������ ���� �丮��Ʈ ����
	Sector& oldSector = GET_INSTANCE(Core)->GetChannel(mChannel).FindSector(oldX, oldY);
	Monster* oldMonsters = oldSector.GetMonsters();
	int index = 0;
	// ������ ���̴� ���� ������ ����
	for (int i = oldSector.GetStartId(); i < oldSector.GetEndId(); ++i, ++index)
	{
		if (CheckDistance(oldMonsters[index].GetX(), oldMonsters[index].GetY()) == false)
		{
			// ���� ���� �þ߿� �־��µ�, ������ ���ٸ�? �����׵� �����, �����׵� �����ߴ�
			if (mViewList.count(i) == true)
			{
				mViewList.erase(i);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, i);
			}

			if (oldMonsters[index].GetViewList().count(myId) == true)
			{
				oldMonsters[index].GetViewList().erase(myId);
			}
		}
		else
		{
			// ���� ���� �þ߿� �־��µ�, ������ ���ٸ�? �����׵� �����, �����׵� �����ߴ�
			if (mViewList.count(i) == true)
			{
				mViewList.erase(i);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, i);
			}

			if (oldMonsters[index].GetViewList().count(myId) == true)
			{
				oldMonsters[index].GetViewList().erase(myId);
			}
		}
	}

	// ���ο� ���Ϳ� ����	
	GET_INSTANCE(Core)->GetChannel(mChannel).PushSectorObject(newX, newY, myId);
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
	//mX = 0;
	//mY = 0;

	mChannelIndex = -1;
	mChannel = -1;

	return true;
}

void Player::PlayerDisconnect()
{
	Sector& sector = GET_INSTANCE(Core)->GetChannel(mChannel).FindSector(mX, mY);

	int myId = mOver->myId;
	Player* users = GET_INSTANCE(Core)->GetUsers();

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

	Monster* monsters = sector.GetMonsters();
	int index = 0;
	for (int i = sector.GetStartId(); i < sector.GetEndId(); ++i, ++index)
	{
		//int index = sector.GetObjectIndex(i);
		//int index = GET_INSTANCE(Core)->GetChannel(mChannel).FindSectorObjectIndex(mX, mY, i);
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
	// ���� �������� �����־����
	GET_INSTANCE(Core)->SendAddObjectPacket(myId, myId);

	Player* users = GET_INSTANCE(Core)->GetUsers();

	// �˻��ؾ��� ���� ���̵� ���
	std::vector<int> sectorUserIds = GET_INSTANCE(Core)->GetChannel(mChannel).GetSectorUserIds(mX, mY);

	// ���� ä���� ���� ���� ������Ʈ�� �˻�
	// ���ӵ� �ٸ� �������Ե� �� ������ ����
	for (int i = 0; i < sectorUserIds.size(); ++i)
	{
		int id = sectorUserIds[i];
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
		int x = users[id].GetX();
		int y = users[id].GetY();
		if (CheckDistance(x, y) == false)
		{
			continue;
		}

		// ������ �ٸ��ֵ� ������ ����
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			mViewList.insert(acc, id);
			acc->second = id;
			acc.release();
			GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);
		}
		// �ٸ��ֵ鿡�� ���� ������ ����
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			users[id].GetViewList().insert(acc, myId);
			acc->second = myId;
			acc.release();
			GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
		}
	}

	// ���� ���͸� ã��
	Sector& sector = GET_INSTANCE(Core)->GetChannel(mChannel).FindSector(mX, mY);

	Monster* monsters = sector.GetMonsters();
	int index = 0;
	// ������ ���̴� ���� ������ ����
	for (int i = sector.GetStartId(); i < sector.GetEndId(); ++i, ++index)
	{
		if (CheckDistance(monsters[index].GetX(), monsters[index].GetY()) == false)
		{
			continue;
		}

		// ���̴� NPC�� �����.
		monsters[index].WakeUp();

		tbb::concurrent_hash_map<int, int>::accessor acc;
		mViewList.insert(acc, i);
		acc->second = index;
		acc.release();
		GET_INSTANCE(Core)->SendAddMonsterPacket(myId, i, monsters[index].GetX(), monsters[index].GetY());
	}
}

void Player::CheckViewList()
{
	int myId = mOver->myId;
	std::unordered_map<int, int> newViewList;

	Player* users = GET_INSTANCE(Core)->GetUsers();
	
	// �˻��ؾ��� ���� ���̵� ���
	std::vector<int> sectorUserIds = GET_INSTANCE(Core)->GetChannel(mChannel).GetSectorUserIds(mX, mY);
	// ���� ��ó�� �ִ� ������Ʈ ���̵� ���ο� �丮��Ʈ�� ����
	for (int i = 0; i < sectorUserIds.size(); ++i)
	{
		int id = sectorUserIds[i];
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

		int x = users[id].GetX();
		int y = users[id].GetY();
		if (CheckDistance(x, y) == false)
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
		newViewList.emplace(id, id);
	}
	
	Sector& sector = GET_INSTANCE(Core)->GetChannel(mChannel).FindSector(mX, mY);

	// ���� ������ ���͸� �˻�
	Monster* monsters = sector.GetMonsters();
	int index = 0;
	for (int i = sector.GetStartId(); i < sector.GetEndId(); ++i, ++index)
	{
		if (CheckDistance(monsters[index].GetX(), monsters[index].GetY()) == false)
		{
			// ���� ���� �þ߿� �־��µ�, ������ ���ٸ�? �����׵� �����, �����׵� �����ߴ�
			if (mViewList.count(i) == true)
			{
				mViewList.erase(i);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, i);
			}

			if (monsters[index].GetViewList().count(myId) == true)
			{
				monsters[index].GetViewList().erase(myId);
			}

			continue;
		}
		newViewList.emplace(i, index);
	}

	// ���� ������ ��ó�� �ִ� ������Ʈ�鿡 ����
	for (auto obj : newViewList)
	{
		int id = obj.first;
		//int index = sector.GetObjectIndex(id);
		int index = obj.second;

		// ������ ���� ������Ʈ���
		if (mViewList.count(id) == false)
		{			
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				mViewList.insert(acc, id);
				acc->second = index;
				acc.release();
			}

			// ���Ͷ��
			if (id >= MONSTER_START_ID)
			{
				// ���Ͷ�� �����.
				monsters[index].WakeUp();
				GET_INSTANCE(Core)->SendAddMonsterPacket(myId, id, monsters[index].GetX(), monsters[index].GetY());
				continue;
			}
			else
			{
				GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);

				// ���� ������ �丮��Ʈ �˻�
				if (users[id].GetViewList().count(myId) == false)
				{
					tbb::concurrent_hash_map<int, int>::accessor acc;
					users[id].GetViewList().insert(acc, myId);
					acc->second = myId;
					acc.release();
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
				monsters[index].WakeUp();
				continue;
			}

			// ���� ������ �丮��Ʈ �˻�
			if (users[id].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[id].GetViewList().insert(acc, myId);
				acc->second = myId;
				GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
			}
			else
			{
				GET_INSTANCE(Core)->SendPositionPacket(id, myId);
			}
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

	Sector& oldSector = GET_INSTANCE(Core)->GetChannel(oldChannel).FindSector(mX, mY);
	Monster* oldMonsters = oldSector.GetMonsters();
	for (int i = oldSector.GetStartId(); i < oldSector.GetEndId(); ++i)
	{
		//int index = oldSector.GetObjectIndex(i);
		int index = GET_INSTANCE(Core)->GetChannel(oldChannel).FindSectorObjectIndex(mX, mY, i);

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

	Sector& newSector = GET_INSTANCE(Core)->GetChannel(newChannel).FindSector(mX, mY);
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

		int x = users[id].GetX();
		int y = users[id].GetY();
		if (CheckDistance(x, y) == false)
		{
			continue;
		}

		// ������ �ٸ��ֵ� ������ ����
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			mViewList.insert(acc, id);
			acc->second = id;
			acc.release();
			GET_INSTANCE(Core)->SendAddObjectPacket(myId, id);
		}
		// �ٸ��ֵ鿡�� ���� ������ ����
		{
			tbb::concurrent_hash_map<int, int>::accessor acc;
			users[id].GetViewList().insert(acc, myId);
			acc->second = myId;
			acc.release();
			GET_INSTANCE(Core)->SendAddObjectPacket(id, myId);
		}
	}

	Monster* newMonsters = newSector.GetMonsters();
	// ������ ���̴� ���� ������ ����
	for (int i = newSector.GetStartId(); i < newSector.GetEndId(); ++i)
	{
		//int index = oldSector.GetObjectIndex(i);
		int index = GET_INSTANCE(Core)->GetChannel(newChannel).FindSectorObjectIndex(mX, mY, i);
		if (CheckDistance(newMonsters[index].GetX(), newMonsters[index].GetY()) == false)
		{
			continue;
		}

		// ���̴� NPC�� �����.
		newMonsters[index].WakeUp();

		tbb::concurrent_hash_map<int, int>::accessor acc;
		mViewList.insert(acc, i);
		acc->second = index;
		acc.release();
		GET_INSTANCE(Core)->SendAddMonsterPacket(myId, i, newMonsters[index].GetX(), newMonsters[index].GetY());
	}
}

void Player::ProcessChat(wchar_t* chat)
{
	int myId = mOver->myId;
	Player* users = GET_INSTANCE(Core)->GetUsers();

	std::unordered_set<int> newViewList;

	// �˻��ؾ��� ���� ���̵� ���
	std::vector<int> sectorUserIds = GET_INSTANCE(Core)->GetChannel(mChannel).GetSectorUserIds(mX, mY);
	// ���� ��ó�� �ִ� ������Ʈ ���̵� ���ο� �丮��Ʈ�� ����
	for (int i = 0; i < sectorUserIds.size(); ++i)
	{
		int id = sectorUserIds[i];
		if (id == myId)
		{
			continue;
		}

		if (users[id].GetIsConnect() == false)
		{
			continue;
		}

		int x = users[id].GetX();
		int y = users[id].GetY();
		if (CheckDistance(x, y) == false)
		{
			continue;
		}

		newViewList.emplace(id);
	}

	for (auto& id : newViewList)
	{
		GET_INSTANCE(Core)->SendChatPacket(id, myId, chat);
	}
}

void Player::ProcessAttack()
{
	int myId = mOver->myId;
	std::unordered_map<int, int> newViewList;

	Sector& sector = GET_INSTANCE(Core)->GetChannel(mChannel).FindSector(mX, mY);
	// ���� ������ ���͸� �˻�
	Monster* monsters = sector.GetMonsters();
	int index = 0;
	for (int i = sector.GetStartId(); i < sector.GetEndId(); ++i, ++index)
	{
		if (CheckDistance(monsters[index].GetX(), monsters[index].GetY()) == false)
		{
			// ���� ���� �þ߿� �־��µ�, ������ ���ٸ�? �����׵� �����, �����׵� �����ߴ�
			if (mViewList.count(i) == true)
			{
				mViewList.erase(i);
				GET_INSTANCE(Core)->SendRemoveObjectPacket(myId, i);
			}

			if (monsters[index].GetViewList().count(myId) == true)
			{
				monsters[index].GetViewList().erase(myId);
			}

			continue;
		}
		newViewList.emplace(i, index);
	}

	for (auto& obj : newViewList)
	{
		int index = obj.second;
		if (monsters[index].CheckCollision(mX, mY) == true)
		{
			monsters[index].SetTargetId(myId);
		}
	}

}

Monster::Monster()
	: Character()
{
	mRangeMin = std::make_pair(0, 0);
	mRangeMax = std::make_pair(0, 0);

	mSectorXId = 0;
	mSectorYId = 0;
	mState = MONSTER_STATE::SLEEP;

	mTargetId = -1;
}

Monster::~Monster()
{
	Reset();
}

void Monster::Reset()
{
	Character::Reset();
	//mSectorId = std::make_pair(0, 0);
	mSectorXId = 0;
	mSectorYId = 0;
	mState = MONSTER_STATE::SLEEP;
	mTargetId = -1;
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

	mRangeMin = std::make_pair(0, 0);
	mRangeMax = std::make_pair(0, 0);

	mSectorXId = 0;
	mSectorYId = 0;

	mState = MONSTER_STATE::SLEEP;
	mTargetId = -1;

	return true;
}

void Monster::ProcessMove(char dir)
{
	int x = mX;
	int y = mY;

	if (mTargetId != -1)
	{
		Player* users = GET_INSTANCE(Core)->GetUsers();
		int targetX = users[mTargetId].GetX();
		int targetY = users[mTargetId].GetY();
		if (x < targetX)
		{
			++x;
		}
		else if (x > targetX)
		{
			--x;
		}
		else if (y < targetY)
		{
			++y;
		}
		else if(y > targetY)
		{
			--y;
		}

		if (CheckRange(x, y) == false)
		{
			return;
		}

		mX = x;
		mY = y;
		return;
	}

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

bool Monster::CheckRange(int x, int y)
{
	if (x < mRangeMin.first || x > mRangeMax.first || y < mRangeMin.second || y > mRangeMax.second)
	{
		return false;
	}

	return true;
}

using namespace std;
void Monster::WakeUp()
{
	if (mState != MONSTER_STATE::SLEEP)
	{
		return;
	}

	mState = MONSTER_STATE::READY;

	MoveEvent();
}

void Monster::MoveEvent()
{
	int myId = mOver->myId;
	// Ÿ�̸ӿ� 1�ʵڿ� move ����
	std::chrono::seconds sec{ 1 };

	int sectorX = mSectorXId;
	int sectorY = mSectorYId;

	GET_INSTANCE(GameTimer)->AddTimer(std::chrono::high_resolution_clock::now() + 1s, SERVER_EVENT::MONSTER_MOVE, myId, mChannel, sectorX, sectorY);
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
	//Sector& sector = GET_INSTANCE(Core)->GetChannel(mChannel).FindSector(mX, mY);

	int myId = mOver->myId;
	int myIndex = GET_INSTANCE(Core)->GetChannel(mChannel).FindSectorObjectIndex(mX, mY, myId);

	std::unordered_set<int> newViewList;
	Player* users = GET_INSTANCE(Core)->GetUsers();

	std::vector<int> sectorUserIds = GET_INSTANCE(Core)->GetChannel(mChannel).GetSectorUserIds(mX, mY, false);
	// ���� ��ó�� �ִ� ������Ʈ ���̵� ���ο� �丮��Ʈ�� ����
	for (int i = 0; i < sectorUserIds.size(); ++i)
	{
		int id = sectorUserIds[i];
		if (users[id].GetIsConnect() == false)
		{
			// ���� ���� �����߾��µ�, ������ ���������ߴ��ְ� �����ִٸ�? ���������� �����.
			if (mViewList.count(id) == true)
			{
				mViewList.erase(id);
			}
			continue;
		}

		int x = users[id].GetX();
		int y = users[id].GetY();
		if (CheckDistance(x, y) == false)
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
				acc.release();
			}

			// ���� ������ �丮��Ʈ �˻�
			if (users[id].GetViewList().count(myId) == false)
			{
				tbb::concurrent_hash_map<int, int>::accessor acc;
				users[id].GetViewList().insert(acc, myId);
				acc->second = myIndex;
				acc.release();
				GET_INSTANCE(Core)->SendAddMonsterPacket(id, myId, mX, mY);
			}
			else
			{
				GET_INSTANCE(Core)->SendMonsterPositionPacket(id, myId, mX, mY);
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
				acc.release();
				GET_INSTANCE(Core)->SendAddMonsterPacket(id, myId, mX, mY);
			}
			else
			{
				GET_INSTANCE(Core)->SendMonsterPositionPacket(id, myId, mX, mY);
			}
		}
	}

	// �þ߿� �÷��̾ ������ ��� �����δ�.
	if (newViewList.size() > 0)
	{
		MoveEvent();
	}
	else
	{
		mTargetId = -1;
		mState = MONSTER_STATE::SLEEP;
	}
}

bool Monster::CheckCollision(int x, int y)
{
	if (x - 1 <= mX && y == mY)
	{
		return true;
	}
	else if (x + 1 >= mX && y == mY)
	{
		return true;
	}
	else if (x == mX && y - 1 <= mY)
	{
		return true;
	}
	else if (x == mX && y + 1 >= mY)
	{
		return true;
	}

	return false;
}

FollowingMonster::FollowingMonster()
{
}

FollowingMonster::~FollowingMonster()
{
}

void FollowingMonster::Reset()
{
}

bool FollowingMonster::Inititalize(int id)
{
	return false;
}
