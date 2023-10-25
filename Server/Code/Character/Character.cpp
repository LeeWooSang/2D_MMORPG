#include "Character.h"

Character::Character()
	: mOverEx(nullptr)
{
	mOverEx = new OverEx;
	mOverEx->eventType = SERVER_EVENT::RECV;
}

Character::~Character()
{
	delete mOverEx;
	mOverEx = nullptr;
}

Player::Player()
	: mSocket(0), mPrevSize(0), mSendBytes(0), mConnect(false)
{
	memset(mPacketBuf, 0, sizeof(mPacketBuf));
}

Player::~Player()
{
	closesocket(mSocket);
	mPrevSize = 0;
	mSendBytes = 0;
	memset(mPacketBuf, 0, sizeof(mPacketBuf));
	mConnect = false;	
}
