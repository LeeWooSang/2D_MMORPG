#include "../Code/pch.h"
#include "Character.h"

Character::Character()
	: mOverEx(nullptr)
{
}

Character::~Character()
{
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
