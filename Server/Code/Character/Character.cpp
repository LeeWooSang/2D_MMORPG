#include "Character.h"

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
	mOver->myId = 0;
}

bool Character::Inititalize()
{
	mOver = new Over;
	mOver->eventType = SERVER_EVENT::DEFAULT;
	mOver->myId = 0;

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
	over->myId = 0;

	memset(over->messageBuffer, 0, sizeof(MAX_BUFFER));
	over->dataBuffer.buf = over->messageBuffer;
	over->dataBuffer.len = MAX_BUFFER;

	mPrevSize = 0;
	mSendBytes = 0;
	memset(mPacketBuf, 0, sizeof(mPacketBuf));
	mConnect = false;
	closesocket(mSocket);
}

bool Player::Inititalize()
{
	OverEx* over = new OverEx;
	memset(&over->overlapped, 0, sizeof(WSAOVERLAPPED));	
	over->eventType = SERVER_EVENT::RECV;
	over->myId = 0;

	memset(over->messageBuffer, 0, sizeof(MAX_BUFFER));
	over->dataBuffer.buf = over->messageBuffer;
	over->dataBuffer.len = MAX_BUFFER;

	mOver = over;

	mPrevSize = 0;
	mSendBytes = 0;
	memset(mPacketBuf, 0, sizeof(mPacketBuf));
	mConnect = false;
	mSocket = 0;

	return true;
}

void Player::ProcessMove(char dir)
{
	mCharacterMtx.lock();
	int x = mX;
	int y = mY;
	mCharacterMtx.unlock();

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

	mCharacterMtx.lock();
	mX = x;
	mY = y;
	mCharacterMtx.unlock();
}
